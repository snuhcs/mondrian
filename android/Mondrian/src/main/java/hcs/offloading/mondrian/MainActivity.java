package hcs.offloading.mondrian;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.ImageView;

import org.json.JSONException;

import java.io.IOException;

import hcs.offloading.mondrian.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {
    private MondrianApp app;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(ActivityMainBinding.inflate(getLayoutInflater()).getRoot());

        ImageView outputView = findViewById(R.id.outputView);

        try {
            app = new MondrianApp(outputView);
        } catch (JSONException | IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        app.close();
    }
}
