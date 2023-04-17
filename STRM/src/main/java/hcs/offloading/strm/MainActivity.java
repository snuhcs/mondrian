package hcs.offloading.strm;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.ImageView;

import org.json.JSONException;

import java.io.IOException;

import hcs.offloading.strm.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {
    private Emulator emulator;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(ActivityMainBinding.inflate(getLayoutInflater()).getRoot());

        ImageView outputView = findViewById(R.id.outputView);

        try {
            emulator = new Emulator(outputView);
        } catch (JSONException | IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        emulator.close();
    }
}
