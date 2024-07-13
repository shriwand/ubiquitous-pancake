package com.ubiquitous_pancake.callcapture

import android.content.Intent
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.ubiquitous_pancake.callcapture.databinding.ActivityMainBinding


class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        binding.sampleText.text = stringFromJNI()
        startService(Intent(this, CallRecordingService::class.java))
    }

    /**
     * A native method that is implemented by the 'callcapture' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'callcapture' library on application startup.
        init {
            System.loadLibrary("callcapture")
        }
    }
}