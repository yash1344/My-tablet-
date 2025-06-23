package com.example.mytablettcp

import android.content.Context
import android.graphics.Bitmap
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.unit.dp
import com.example.mytablettcp.ui.theme.MyTabletTcpTheme
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

val TAG: String = "MainActivity"

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()


        setContent {
            MyTabletTcpTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    Greeting(
                        this,
                        name = "Android",
                        modifier = Modifier
                            .padding(innerPadding)
                            .fillMaxSize()
                    )
                }
            }
        }
    }
}

@Composable
fun Greeting(context: Context, name: String, modifier: Modifier = Modifier) {
    val coroutineScope = rememberCoroutineScope()
    var receivedBitmap by remember { mutableStateOf<Bitmap?>(null) }
    val tcpClient = remember {
        TcpClient(context, "192.168.0.101", 5000)
    }
    var isStreaming by remember { mutableStateOf(false) }

    /*LaunchedEffect(isStreaming) {
        if (isStreaming) {
            Log.d(TAG, "Greeting isStreaming: $isStreaming")
            coroutineScope.launch(Dispatchers.IO) {
                tcpClient.startImageAcquisition { bytes ->
                    Log.d(TAG, "startImageAcquisition: ")
                    val bitmap = BitmapFactory.decodeByteArray(bytes, 0, bytes.size)

                    coroutineScope.launch(Dispatchers.Main) {
                        receivedBitmap = bitmap
                    }
                }
            }
        }
    }*/

    /*LaunchedEffect(isStreaming) {
        coroutineScope.launch(Dispatchers.IO) {
            tcpClient.imageChannel.receiveAsFlow().collect { bytes ->
                val bitmap = BitmapFactory.decodeByteArray(bytes, 0, bytes.size)
                withContext(Dispatchers.Main) {
                    receivedBitmap = bitmap
                }
            }
        }
    }*/

    /* LaunchedEffect(isStreaming) {
         Log.d(
             "MainActivity",
             "Greeting: isStreaming: $isStreaming isConnected: ${tcpClient.isConnected}"
         )
         while (isStreaming and tcpClient.isConnected) {
             Log.d(TAG, "While: start")
             receivedBitmap = tcpClient.receiveImage()
             delay(16)
             Log.d(TAG, "While: end")
         }
     }*/

    Column(
        modifier = modifier,
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Text(text = "Hello $name!")

        Button(onClick = {
            coroutineScope.launch {
                tcpClient.connect()
//                isStreaming = true
//                receivedBitmap = tcpClient.receiveImage() // Adjust size dynamically if needed
            }
        }) {
            Text(text = "Connect")
        }

        Button(onClick = {
            isStreaming = true
            coroutineScope.launch(Dispatchers.IO) {
                tcpClient.startImageAcquisition { bitmap ->
                    receivedBitmap = bitmap
                }
            }
            coroutineScope.launch(Dispatchers.IO) {
                tcpClient.startSendingCoordinates()
            }
        }) {
            Text(text = "Start Streaming")
        }

        Button(onClick = {
            isStreaming = false
        }) {
            Text(text = "Stop Streaming")
        }

        Spacer(modifier = Modifier.height(16.dp))

        if (receivedBitmap != null) {
            Image(
                bitmap = receivedBitmap!!.asImageBitmap(),
                contentDescription = "Received Image",
                modifier = Modifier
                    .fillMaxSize(),
                contentScale = ContentScale.Fit
            )
        } else {
            Text(text = "No Image Received", color = Color.Red)
        }
    }
}