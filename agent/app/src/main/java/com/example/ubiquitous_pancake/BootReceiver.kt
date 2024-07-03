package com.example.ubiquitous_pancake

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.util.Log

class BootReceiver : BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent) {
        Log.i(TAG, "Intent received. Attempting to process ACTION_BOOT_COMPLETED. Try to start");
        if (intent.action == Intent.ACTION_BOOT_COMPLETED) {
            val serviceIntent = Intent(context, MainService::class.java)
            context.startService(serviceIntent)
        }
    }

    companion object{
        private const val TAG = "BootReceiver";
    }
}
