package com.example.ubiquitous_pancake

import android.app.Service
import android.content.Intent
import android.os.IBinder
import android.util.Log


class MainService : Service() {

    companion object {
        private  const val TAG = "MainService"
        private const val ACTION_ECHO = "com.example.app.action.ECHO"
        private const val EXTRA_MESSAGE = "com.example.app.extra.MESSAGE"
        init {
            System.loadLibrary("ubiquitous_pancake")
        }
    }

    override fun onCreate() {
        super.onCreate()
        Log.d(TAG, "OnCreate")
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        Log.d(TAG, "OnStart")
        intent?.let { handleEchoIntent(it) }
        return START_STICKY
    }

    override fun onBind(intent: Intent?): IBinder? {
        Log.d(TAG, "OnBind")
        return null
    }

    private fun handleEchoIntent(intent: Intent) {
        if (intent.action == ACTION_ECHO) {
            val message = intent.getStringExtra(EXTRA_MESSAGE)
            Log.d(TAG, "Received message: $message")
        }
    }

    /**
     * A native method that is implemented by the 'ubiquitous_pancake' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

}