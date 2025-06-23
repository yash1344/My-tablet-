package com.example.mytablettcp

import android.content.Context
import android.graphics.BitmapFactory
import android.util.Log
import android.widget.Toast
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.channels.Channel
import kotlinx.coroutines.flow.receiveAsFlow
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import kotlinx.coroutines.withTimeoutOrNull
import java.io.IOException
import java.io.InputStream
import java.io.OutputStream
import java.net.Socket
import kotlin.math.min

class TcpClient1(private val context: Context, private val ip: String, private val port: Int) {
    private val TAG = "TcpClient"
    private lateinit var outputStream: OutputStream
    private lateinit var inputStream: InputStream
    var isConnected = false
    private var socket: Socket? = null

    // Image processing pipeline
    val imageChannel = Channel<ByteArray>(Channel.UNLIMITED)
    private val bufferSize = 8192  // Optimized buffer size

    // Header constants
    private val headerStart = "imgS:{".toByteArray()
    private val headerEnd = '}'.code.toByte()

    suspend fun connect() {
        withContext(Dispatchers.IO) {
            try {
                socket = Socket(ip, port).also {
                    outputStream = it.getOutputStream()
                    inputStream = it.getInputStream()
                    isConnected = true
                    Log.d(TAG, "Connected to server")
                    withContext(Dispatchers.Main) {
                        Toast.makeText(context, "Connected", Toast.LENGTH_SHORT).show()
                    }
                    startImagePipeline()
                }
            } catch (e: Exception) {
                Log.e(TAG, "Connection failed: ${e.message}")
            }
        }
    }

    fun CoroutineScope.startImagePipeline() {
        // Launch data receiver
        launch {
            val buffer = ByteArray(bufferSize)
            var leftovers = ByteArray(0)

            while (isActive && isConnected) {
                try {
                    val bytesRead = withTimeoutOrNull(30_000) {
                        inputStream.read(buffer)
                    } ?: throw IOException("Read timeout")

                    if (bytesRead == -1) break

                    val chunk = leftovers + buffer.copyOf(bytesRead)
                    var ptr = 0

                    while (ptr < chunk.size) {
                        val headerPos = findHeaderPosition(chunk, ptr)
                        if (headerPos == -1) {
                            leftovers = chunk.copyOfRange(ptr, chunk.size)
                            break
                        }

                        val size = parseSize(chunk, headerPos)
                        ptr =
                            headerPos + headerStart.size + size.toString().length + 1  // Skip header

                        val imageData = readImageData(chunk, ptr, size)
                        ptr += imageData.second
                        imageChannel.send(imageData.first)
                    }
                } catch (e: Exception) {
                    Log.e(TAG, "Pipeline error: ${e.message}")
                    disconnect()
                }
            }
        }

        // Launch image processor
        launch(Dispatchers.Default) {
            imageChannel.receiveAsFlow().collect { bytes ->
                val bitmap = BitmapFactory.decodeByteArray(bytes, 0, bytes.size)
                // Send to UI via callback or ViewModel
            }
        }
    }

    private fun findHeaderPosition(buffer: ByteArray, start: Int): Int {
        var i = start
        val maxPos = buffer.size - headerStart.size
        while (i <= maxPos) {
            if (buffer[i] == headerStart[0]) {
                var match = true
                for (j in 1 until headerStart.size) {
                    if (buffer[i + j] != headerStart[j]) {
                        match = false
                        break
                    }
                }
                if (match) return i
            }
            i++
        }
        return -1
    }

    private fun parseSize(buffer: ByteArray, headerPos: Int): Int {
        val startIndex = headerPos + headerStart.size
        val endIndex = buffer.indexOf(headerEnd, startIndex)

        if (endIndex == -1 || endIndex - startIndex > 15) {
            throw IOException("Invalid size format")
        }

        return try {
            buffer.decodeToString(startIndex, endIndex).toInt()
        } catch (e: NumberFormatException) {
            throw IOException("Invalid numeric format in size")
        }
    }

    // Add this extension function to handle indexOf with start position
    private fun ByteArray.indexOf(element: Byte, start: Int = 0): Int {
        for (i in start until this.size) {
            if (this[i] == element) return i
        }
        return -1
    }

    private suspend fun readImageData(
        buffer: ByteArray,
        start: Int,
        size: Int
    ): Pair<ByteArray, Int> {
        val imageData = ByteArray(size)
        var bytesCopied = 0

        // Copy from current buffer
        val initialCopy = min(size, buffer.size - start)
        System.arraycopy(buffer, start, imageData, 0, initialCopy)
        bytesCopied += initialCopy

        // Read remaining data
        while (bytesCopied < size) {
            val bytesRead = withTimeoutOrNull(30_000) {
                inputStream.read(imageData, bytesCopied, size - bytesCopied)
            } ?: throw IOException("Read timeout")

            if (bytesRead == -1) throw IOException("Stream closed")
            bytesCopied += bytesRead
        }

        return Pair(imageData, initialCopy + (size - initialCopy))
    }

    suspend fun disconnect() {
        isConnected = false
        withContext(Dispatchers.IO) {
            socket?.close()
            imageChannel.close()
        }
    }

    suspend fun send(msg: ByteArray) {
        withContext(Dispatchers.IO) {
            try {
                outputStream.write(msg)
                outputStream.flush()
            } catch (e: IOException) {
                Log.e(TAG, "Send failed: ${e.message}")
                disconnect()
            }
        }
    }
}