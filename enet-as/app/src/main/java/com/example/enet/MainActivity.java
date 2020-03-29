package com.example.enet;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;


public class MainActivity extends AppCompatActivity {

    private ENET enet = new ENET(); //java接口实例化　下面直接利用java函数调用NDK c++函数


    private Bitmap yourSelectedImage = null;

    private static final int SELECT_IMAGE = 1;
    private static final String TAG = MainActivity.class.getName();
    private ImageView show_image;
    private boolean load_result = false;
    private int[] ddims = {1, 3, 512, 288}; //这里的维度的值要和train model的input 一一对应



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);



        try
        {
            initENet();//初始化模型
            Log.e("MainActivity", "initENet ok");
        } catch (IOException e) {
            Log.e("MainActivity", "initENet error");
        }


        init_view();//检测+view画图

    }


    // initialize view
    private void init_view() {
        show_image = (ImageView) findViewById(R.id.show_image);
        Button use_photo = (Button) findViewById(R.id.use_photo);
        use_photo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View arg0) {
                Intent i = new Intent(Intent.ACTION_PICK);
                i.setType("image/*");
                startActivityForResult(i, SELECT_IMAGE);
            }
        });


        Button detect_photo = (Button) findViewById(R.id.detect_photo);
        detect_photo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View arg0) {

                if (yourSelectedImage == null)
                    return;

                predict_image(yourSelectedImage);
            }

        });



    }



    private void initENet() throws IOException {
        byte[] param = null;
        byte[] bin = null;
        {
            //用io流读取二进制文件，最后存入到byte[]数组中
            InputStream assetsInputStream = getAssets().open("enet_512288.param.bin");// param：  网络结构文件
            int available = assetsInputStream.available();
            param = new byte[available];
            int byteCode = assetsInputStream.read(param);
            assetsInputStream.close();
        }
        {
            //用io流读取二进制文件，最后存入到byte上，转换为int型
            InputStream assetsInputStream = getAssets().open("enet_512288.bin");//bin：   model文件
            int available = assetsInputStream.available();
            bin = new byte[available];
            int byteCode = assetsInputStream.read(bin);
            assetsInputStream.close();
        }

        load_result = enet.Init(param, bin);// 再将文件传入java的NDK接口(c++ 代码中的init接口 )
        Log.d("load model", "ENet_load_model_result:" + load_result);
    }



    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        super.onActivityResult(requestCode, resultCode, data);

        if (resultCode == RESULT_OK && null != data) {
            Uri selectedImage = data.getData();

            try
            {
                if (requestCode == SELECT_IMAGE) {
                    Bitmap bitmap = decodeUri(selectedImage);

                    Bitmap rgba = bitmap.copy(Bitmap.Config.ARGB_8888, true);

                    // resize to 512x288
                    yourSelectedImage = Bitmap.createScaledBitmap(rgba, ddims[2], ddims[3], false);

                    show_image.setImageBitmap(yourSelectedImage);
                }
            }
            catch (FileNotFoundException e)
            {
                Log.e("MainActivity", "FileNotFoundException");
                return;
            }
        }
    }

    private Bitmap decodeUri(Uri selectedImage) throws FileNotFoundException
    {
        // Decode image size
        BitmapFactory.Options o = new BitmapFactory.Options();
        o.inJustDecodeBounds = true;
        BitmapFactory.decodeStream(getContentResolver().openInputStream(selectedImage), null, o);

        // The new size we want to scale to
        final int REQUIRED_SIZE = 600;

        // Find the correct scale value. It should be the power of 2.
        int width_tmp = o.outWidth, height_tmp = o.outHeight;
        int scale = 1;
        while (true) {
            if (width_tmp / 2 < REQUIRED_SIZE
                    || height_tmp / 2 < REQUIRED_SIZE) {
                break;
            }
            width_tmp /= 2;
            height_tmp /= 2;
            scale *= 2;
        }

        // Decode with inSampleSize
        BitmapFactory.Options o2 = new BitmapFactory.Options();
        o2.inSampleSize = scale;
        return BitmapFactory.decodeStream(getContentResolver().openInputStream(selectedImage), null, o2);
    }


    //  predict image
    private void predict_image(Bitmap bmp) {
        // picture to float array
        Bitmap rgba = bmp.copy(Bitmap.Config.ARGB_8888, true);
        // resize
        Bitmap input_bmp = Bitmap.createScaledBitmap(rgba, ddims[2], ddims[3], false);


        try {
            // Data format conversion takes too long
            // Log.d("inputData", Arrays.toString(inputData));
            long start = System.currentTimeMillis();
            // get predict result
            float[] result = enet.Process(input_bmp);
            // time end
            long end = System.currentTimeMillis();
            Log.d(TAG, "origin predict result:" + Arrays.toString(result));
            long time = end - start;
            Log.d("result length", "length of result: " + String.valueOf(result.length));


            // 画布配置
            Canvas canvas = new Canvas(input_bmp);
            //图像上画矩形
            Paint paint = new Paint();
            //continue to draw rect
            Log.d(TAG, "result :" + result.length);
            Log.d(TAG, "result :" + Arrays.toString(result));
            for(int num = 0; num < result.length; num++){

                // 画框
                int row =num%ddims[2];
                int col = num/ddims[2];

                if (result[num]==1){
                    paint.setColor(Color.RED);
                    paint.setStyle(Paint.Style.STROKE);//不填充
                    canvas.drawCircle(row, col, 1,  paint);
                }

                if (result[num]==2){
                    paint.setColor(Color.BLUE);
                    paint.setStyle(Paint.Style.STROKE);//不填充
                    canvas.drawCircle(row, col, 1,  paint);
                }
                if (result[num]==3){
                    paint.setColor(Color.GREEN);
                    paint.setStyle(Paint.Style.STROKE);//不填充
                    canvas.drawCircle(row, col, 1,  paint);

                }

            }

            show_image.setImageBitmap(input_bmp);


        } catch (Exception e) {
            e.printStackTrace();
        }
    }


}
