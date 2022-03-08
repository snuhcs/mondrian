package hcs.offloading.sensorapplication;

import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;

public class Config {

    public boolean USE_SAVED_VIDEO = false;
    public int WIDTH = 1920;
    public int HEIGHT = 1080;
    public int FPS = 5;
    public String VIDEO_PATH = "/data/local/tmp";

    Config(String jsonPath) throws IOException, ParseException {

        JSONParser jsonParser = new JSONParser();
        JSONObject jsonObject = (JSONObject) jsonParser.parse(new FileReader(new File(jsonPath)));

        if (jsonObject.containsKey("use_saved_video")) {
            USE_SAVED_VIDEO = Boolean.parseBoolean(String.valueOf(jsonObject.get("use_saved_video")));
        }
        if (jsonObject.containsKey("width")) {
            WIDTH = Integer.parseInt(String.valueOf(jsonObject.get("width")));
        }
        if (jsonObject.containsKey("height")) {
            HEIGHT = Integer.parseInt(String.valueOf(jsonObject.get("height")));
        }
        if (jsonObject.containsKey("fps")) {
            FPS = Integer.parseInt(String.valueOf(jsonObject.get("fps")));
        }
        if (jsonObject.containsKey("video_path")) {
            VIDEO_PATH = String.valueOf(jsonObject.get("video_path"));
        }
    }

}
