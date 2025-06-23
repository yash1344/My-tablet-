@file:Suppress("Since15")

package com.example.mytablettcp

import android.content.Context
import android.util.Log
import android.widget.Toast
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.coroutineScope
import kotlinx.coroutines.withContext
import java.io.ByteArrayOutputStream
import java.io.InputStream
import java.io.OutputStream
import java.net.Socket

class MyTcpClient(private val context: Context, private val ip: String, private val port: Int) {
    private val TAG: String = "TcpClient"
    private lateinit var outputStream: OutputStream
    private lateinit var inputStream: InputStream
    var isConnected: Boolean = false

    private val headerStart = "imgS:{".toByteArray()
    private val headerEnd = '}'.code.toByte()

    private val chunkSize = 16
    private val conti_code = "{7[}".toByteArray()
    private var data_buffer: ByteArray = ByteArray(chunkSize + 4)

    private val imageStream = mutableListOf<ByteArray>()

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

    suspend fun startImageAcquisition(imgReady: (ByteArray) -> Unit) {
        withContext(Dispatchers.IO) {
            val oldImg = ByteArrayOutputStream()
            val nextImg = ByteArrayOutputStream()
            var isOldImgReady = false
            val imgIndexes = mutableListOf<Int>()

            Log.d(TAG, "startImageAcquisition isConnected: $isConnected")
            while (isConnected) {
                if (isOldImgReady) {
                    if (oldImg.size() != 0) {
                        imgReady(oldImg.toByteArray())
                        isOldImgReady = false
                    }
                }
                if (nextImg.size() != 0) {
                    oldImg.reset()
                    oldImg.write(nextImg.toByteArray())
                }


                Log.d(TAG, "rawReadBytes: start")
                val rawReadBytes = inputStream.readNBytes(1024 * 50)
                imgIndexes.clear()
                imgIndexes.addAll(rawReadBytes.findAllIndexes(headerStart))

                Log.d(TAG, "startImageAcquisition: ${oldImg.size()}")
                when (imgIndexes.size) {
                    0 -> {
                        if ((oldImg.size() > 0)) {
                            //append readBytes to oldImg
                            oldImg.write(rawReadBytes)
                        }
                    }

                    1 -> {
                        oldImg.write(
                            rawReadBytes, 0, imgIndexes.first()
                        )
                        isOldImgReady = true

                        nextImg.apply {
                            reset()
                            write(
                                rawReadBytes,
                                imgIndexes.first(),
                                rawReadBytes.size - imgIndexes.first()
                            )
                        }
                    }

                    else -> {
                        oldImg.apply {
                            reset()
                            write(
                                rawReadBytes,
                                imgIndexes[imgIndexes.size - 2],
                                imgIndexes.last() - imgIndexes[imgIndexes.size - 2] - headerStart.size
                            )
                        }
                        isOldImgReady = true

                        nextImg.apply {
                            reset()
                            write(
                                rawReadBytes,
                                imgIndexes.last(),
                                rawReadBytes.size - imgIndexes.last()
                            )
                        }
                    }
                }
            }
        }
    }


    fun ByteArray.findAllIndexes(pattern: ByteArray): List<Int> {
        if (pattern.isEmpty() || pattern.size > this.size) return emptyList()
        val indexes = mutableListOf<Int>()
        var i = 0
        while (i <= this.size - pattern.size) {
            var match = true
            for (j in pattern.indices) {
                if (this[i + j] != pattern[j]) {
                    match = false
                    break
                }
            }
            if (match) {
                indexes.add(i + pattern.size)
            }
            i++
        }
        return indexes
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
}