package hcs.offloading.strm;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.ImageView;

import org.json.JSONException;

import java.io.FileOutputStream;
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

    private void saveBitmap(Bitmap bmp, String filepath) {
        try (FileOutputStream out = new FileOutputStream(filepath)) {
            bmp.compress(Bitmap.CompressFormat.PNG, 100, out);
            // PNG is a lossless format, the compression factor (100) is ignored
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
