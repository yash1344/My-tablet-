package com.example.mytablettcp

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.util.Log
import android.widget.Toast
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.compose.ui.graphics.asImageBitmap
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.coroutineScope
import kotlinx.coroutines.delay
import kotlinx.coroutines.withContext
import java.io.BufferedReader
import java.io.IOException
import java.io.InputStream
import java.io.OutputStream
import java.net.Socket

class TcpClient(private val context: Context, private val ip: String, private val port: Int) {
    private var reader: BufferedReader? = null
    private val TAG: String = "TcpClient"
    private lateinit var outputStream: OutputStream
    private lateinit var inputStream: InputStream
    var isConnected: Boolean = false
    var image by mutableStateOf(
        Bitmap.createBitmap(
            Bitmap.createBitmap(
                200,
                100,
                Bitmap.Config.ARGB_8888
            )
        )
    )

    private val chunkSize = 16
    private val conti_code = "{7[}".toByteArray()
    private var data_buffer: ByteArray = ByteArray(chunkSize + 4)

    suspend fun connect() {
        coroutineScope {
            withContext(Dispatchers.IO) {
                try {
                    // Connect to the server
                    val socket = Socket(ip, port)
                    Log.d("TcpClient", "Connected to server")
                    withContext(Dispatchers.Main) {
                        Toast.makeText(context, "Connected to server", Toast.LENGTH_SHORT).show()
                    }

                    outputStream = socket.getOutputStream()
                    inputStream = socket.getInputStream()

                    isConnected = true
//                    reader = BufferedReader(InputStreamReader(inputStream))

//                    val imageData: Bitmap? = receiveImage()
//
//                    Log.d(TAG, "Successfully decoded JPEG image")
//                    return@withContext imageData?.asImageBitmap()
//                    while (true) {
//                        Log.d(TAG, "received: ${receive()}")
//                    }

//                    var counter = 0
//                    while (true) {
//                        send("Hello from Android side................ ${counter}\n".toByteArray())
////                        send_in_packates("Hello from A $counter", chunkSize)
////                        withContext(Dispatchers.Main) {
////                            Toast.makeText(context, "sent $counter", Toast.LENGTH_SHORT).show()
////                        }
//                        counter++
//                        if (counter % 1000 == 0) {
//                            delay(3000)
//                        }
//                    }
                } catch (e: Exception) {
                    Log.e("TcpClient", "Error: ${e.message}")
                }
            }
        }
    }

    suspend fun send(msg: ByteArray) {
        Log.d("TcpClient", "send: ${msg.decodeToString()}")
        withContext(Dispatchers.IO) {
            outputStream.write(msg)
            outputStream.flush()
        }
    }

    suspend fun startImageAcquisition(onImageReady: (Bitmap) -> Unit) {
        while (true) {
            receiveImage()?.let {
                onImageReady(it)
                Log.d(TAG, "startImageAcquisition: image emited")
            }
            delay(1)
        }
    }

    suspend fun send_in_packates(msg: String, size: Int) {
        val newMsg = msg.padEnd(size, ' ')

        val msgBytes = newMsg.toByteArray()
        val remainingDataSize = msgBytes.size - size
        System.arraycopy(msgBytes, 0, data_buffer, 0, size)
        if (remainingDataSize <= 0) {
            val temp = ByteArray(data_buffer.size - chunkSize) { ' '.code.toByte() }
            System.arraycopy(temp, 0, data_buffer, chunkSize, temp.size)
        }

        if (remainingDataSize > 0) {
            System.arraycopy(conti_code, 0, data_buffer, chunkSize, conti_code.size)
            send(data_buffer)
            val remainingData = newMsg.substring(size)
            send_in_packates(remainingData, size)
        } else {
            send(data_buffer)
        }
    }


    private fun readImageSizeHeader(headerStartMarker: String, headerEndMarker: Char): Int {
        val buffer = StringBuilder()
        var foundHeaderStart = false

        // Read until "imgS:{" is found
        while (true) {
            val byte = inputStream.read()
            if (byte == -1) throw IOException("Stream closed before header")

            buffer.append(byte.toChar())

            // Check if we've found the start marker
            if (buffer.endsWith(headerStartMarker)) {
                foundHeaderStart = true
                buffer.clear()  // Reset buffer to collect size
                break
            }

            // Prevent buffer overflow from garbage data
            if (buffer.length > headerStartMarker.length) {
                buffer.deleteCharAt(0)
            }
        }

        // Now read until closing '}' to get the size
        while (foundHeaderStart) {
            val byte = inputStream.read()
            if (byte == -1) throw IOException("Stream closed during size read")

            if (byte == headerEndMarker.code) {
                val sizeStr = buffer.toString()
                return try {
                    sizeStr.toInt()
                } catch (e: NumberFormatException) {
                    throw IOException("Invalid size format: $sizeStr")
                }
            }

            buffer.append(byte.toChar())

            // Prevent unreasonably long size strings
            if (buffer.length > 15) {  // Max 15 digits (supports up to 999TB for 1 byte/pixel)
                throw IOException("Size string too long")
            }
        }

        throw IOException("Failed to read valid image size header")
    }

    private fun readUntilDelimiter(delimiter: Int): String {
        val buffer = StringBuilder()
        var byte = inputStream.read()
        while (byte != -1 && byte != delimiter) {
            buffer.append(byte.toChar())
            byte = inputStream.read()
        }
        return buffer.toString()
    }

    fun receiveImage(): Bitmap? {
        try {
//            Log.d(TAG, "receiveImage: start")

//                val header = readUntilDelimiter('\n'.code)
//                Log.d(TAG, "header: $header")
//                val sizeStr = header.removePrefix("imgSize:").trim()
            val imageSize = readImageSizeHeader("imgS:{", '}')
//            Log.d(TAG, "header: $imageSize")

            // Read image data
            val imageData = readExactBytes(imageSize)

            // Decode bitmap
            val bitmap = BitmapFactory.decodeByteArray(imageData, 0, imageSize)
            bitmap?.asImageBitmap() // Use in UI
//            Log.d(TAG, "receiveImage: end")
            return bitmap
        } catch (e: Exception) {
//            Log.e(TAG, "Error receiving image: ")
            return null
        }
    }

    private fun readExactBytes(size: Int): ByteArray {
        val data = ByteArray(size)
        var read = 0
        while (read < size) {
            val chunk = inputStream.read(data, read, size - read)
            if (chunk == -1) throw IOException("Stream closed")
            read += chunk
//            Log.d(TAG, "readExactBytes: $read/$size")
        }
//        Log.d(TAG, "readExactBytes: Image read complete")
        return data
    }

    suspend fun startSendingCoordinates() {
        while (true) {
            sendCoordinates()
            delay(5)
        }
    }

    suspend fun sendCoordinates(){
        withContext(Dispatchers.IO) {
            outputStream.write("123,456\n".toByteArray())
            Log.d(TAG, "sendCoordinates: sended")
        };
    }
}