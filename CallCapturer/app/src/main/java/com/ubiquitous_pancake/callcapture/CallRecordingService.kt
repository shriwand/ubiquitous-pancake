package com.ubiquitous_pancake.callcapture

import android.R.attr
import android.app.Service
import android.content.Intent
import android.os.Environment
import android.os.IBinder
import android.util.Log
import java.io.BufferedReader
import java.io.DataOutputStream
import java.io.File
import java.io.IOException
import java.io.InputStreamReader


class CallRecordingService : Service() {
    override fun onCreate() {
        super.onCreate()
        startRecording()
    }

    override fun onStartCommand(intent: Intent, flags: Int, startId: Int): Int {
        return START_STICKY
    }
    private fun startRecording() {
        Thread {
            try {
                val process = Runtime.getRuntime().exec("sh")
                val os = DataOutputStream(process.outputStream)

                // Selinux permissive mode
                os.writeBytes(" setenforce 0")
                os.flush()


//                if(tinymix() != 0){
//                    Log.e(TAG, "tinymix() failed")
//                    return@Thread
//                }

                val outputFile = File(
                    Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS),
                    "call_recording.wav"
                )

                os.flush()
                os.close()
                process.waitFor()

                if (outputFile.exists()) {
                    outputFile.delete()
                }

                if(tinycap(outputFile.absolutePath) != 0)
                {
                    Log.e(TAG, "tinycap() failed")
                }
            } catch (e: IOException) {
                e.printStackTrace()
            } catch (e: InterruptedException) {
                e.printStackTrace()
            }
        }.start()
    }

    private fun stopRecording() {
        stopTinycapCapturing()
    }

    external private fun tinymix(): Int
    external private fun tinycap(path: String): Int
    external private fun stopTinycapCapturing(): Void

    override fun onDestroy() {
        super.onDestroy()
        stopRecording()
    }

    override fun onBind(intent: Intent): IBinder? {
        return null
    }

    companion object {
        // Used to load the 'callcapture' library on application startup.
        init {
            System.loadLibrary("callcapture")
        }
        val TAG: String = "native-code"
    }
}

