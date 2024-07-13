package com.ubiquitous_pancake.callcapture

import android.app.Service
import android.content.Intent
import android.os.Environment
import android.os.IBinder
import java.io.DataOutputStream
import java.io.File
import java.io.IOException


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
                val process = Runtime.getRuntime().exec("su")
                val os = DataOutputStream(process.outputStream)

                // Selinux permissive mode
                os.writeBytes(" setenforce 0")

                os.writeBytes("tinymix 'Capture Mixer' 1\n")

                val outputFile = File(
                    Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS),
                    "call_recording.wav"
                )
                os.writeBytes("tinycap " + outputFile.absolutePath + "\n")
                os.flush()
                os.close()
                process.waitFor()
            } catch (e: IOException) {
                e.printStackTrace()
            } catch (e: InterruptedException) {
                e.printStackTrace()
            }
        }.start()
    }

    private fun stopRecording() {
        Thread {
            try {
                val process = Runtime.getRuntime().exec("su")
                val os = DataOutputStream(process.outputStream)
                os.writeBytes("pkill -f tinycap\n")
                os.writeBytes("exit\n")
                os.flush()
                os.close()
                process.waitFor()
            } catch (e: IOException) {
                e.printStackTrace()
            } catch (e: InterruptedException) {
                e.printStackTrace()
            }
        }.start()
    }


    override fun onDestroy() {
        super.onDestroy()
        stopRecording()
    }

    override fun onBind(intent: Intent): IBinder? {
        return null
    }
}

