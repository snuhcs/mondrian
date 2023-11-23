package hcs.offloading.mondrian;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.SurfaceView;
import android.widget.ImageView;
import android.widget.TextView;

import org.json.JSONException;

import java.io.IOException;
import java.util.List;

import hcs.offloading.mondrian.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {
    private MondrianApp app;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(ActivityMainBinding.inflate(getLayoutInflater()).getRoot());

        List<SurfaceView> inputViews = List.of(findViewById(R.id.inputView0), findViewById(R.id.inputView1));
        ImageView outputView = findViewById(R.id.outputView);
        TextView fpsView = findViewById(R.id.fpsView);
        TextView frameCountView = findViewById(R.id.frameCountView);
        TextView totalFramesView = findViewById(R.id.totalFramesView);

        try {
            app = new MondrianApp(inputViews, outputView, fpsView, frameCountView, totalFramesView);
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
