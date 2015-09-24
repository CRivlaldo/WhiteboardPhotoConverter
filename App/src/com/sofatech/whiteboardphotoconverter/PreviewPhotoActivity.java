package com.sofatech.whiteboardphotoconverter;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

public class PreviewPhotoActivity extends Activity {
	
	static final int REQUEST_CROP_IMAGE = 2;
	
	private Uri imageUri = null; 
	private String mTesseractDataLocation = "";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_preview_photo);
		
		Intent intent = getIntent();
		//Bitmap bitmap = (Bitmap)intent.getExtras().get("bitmap");
		//ImageView imageView = (ImageView) findViewById(R.id.imageView);
		//imageView.setImageBitmap(bitmap);
		imageUri = Uri.parse(intent.getExtras().getString("imageUri"));
		mTesseractDataLocation = intent.getExtras().getString("tesseractDataLocation");
		setPicture(imageUri);		
		
		Log.d("preview", "open Uri: " + imageUri.toString());//!!!vladimir: debug
		//
		
		final Button buttonConvert = (Button) findViewById(R.id.buttonConvert);
		buttonConvert.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
	    		convertImage();
			}});
		
		final Button buttonCrop = (Button) findViewById(R.id.buttonCrop);
		buttonCrop.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				cropCapturedImage(imageUri);
			}});	
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.preview_photo, menu);
		return true;
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if(requestCode == REQUEST_CROP_IMAGE && resultCode == RESULT_OK) {
			//Create an instance of bundle and get the returned data
			Bundle extras = data.getExtras();
			// get the cropped bitmap from extras
			Bitmap picture = extras.getParcelable("data");
			// set image bitmap to image view
			ImageView imageView = (ImageView) findViewById(R.id.imageView);	
			imageView.setImageBitmap(picture);
		}
	}

//	private void setPicture(String imagePath) {
//		ImageView imageView = (ImageView) findViewById(R.id.imageView);
//		
//	    // Get the dimensions of the View
//	    int targetW = imageView.getWidth();
//	    int targetH = imageView.getHeight();
//	    
//	    if(targetW == 0 || targetH == 0)
//	    {
//			Log.v(getString(R.string.app_name), "Image view has zero size.");
//			return;
//	    }
//
//	    // Get the dimensions of the bitmap
//	    BitmapFactory.Options bmOptions = new BitmapFactory.Options();
//	    bmOptions.inJustDecodeBounds = true;
//	    BitmapFactory.decodeFile(imagePath, bmOptions);
//	    int photoW = bmOptions.outWidth;
//	    int photoH = bmOptions.outHeight;
//
//	    // Determine how much to scale down the image
//	    int scaleFactor = Math.min(photoW/targetW, photoH/targetH);
//
//	    // Decode the image file into a Bitmap sized to fill the View
//	    bmOptions.inJustDecodeBounds = false;
//	    bmOptions.inSampleSize = scaleFactor;
//	    bmOptions.inPurgeable = true;
//
//	    Bitmap bitmap = BitmapFactory.decodeFile(imagePath, bmOptions);
//	    imageView.setImageBitmap(bitmap);
//	}

	private void setPicture(Uri imageUri) { 
	    BitmapFactory.Options bmOptions = new BitmapFactory.Options();
	    bmOptions.inJustDecodeBounds = false;
	    //Bitmap bitmap = BitmapFactory.decodeFile(imageUri, bmOptions);
	    try {
		    Bitmap bitmap = MediaStore.Images.Media.getBitmap(this.getContentResolver(), imageUri);
		    
			ImageView imageView = (ImageView) findViewById(R.id.imageView);	 
		    imageView.setImageBitmap(bitmap);
	    }
	    catch (Exception e) {
	    	Log.e("setPicture", e.getMessage());
	    }
	}
	
	// create helping method cropCapturedImage(Uri picUri)
	private void cropCapturedImage(Uri picUri) {
		// call the standard crop action intent
		Intent cropIntent = new Intent("com.android.camera.action.CROP");
		// indicate image type and Uri of image
		cropIntent.setDataAndType(picUri, "image/*");
		// set crop properties
		cropIntent.putExtra("crop", "true");
//		// indicate aspect of desired crop
//		cropIntent.putExtra("aspectX", 1);
//		cropIntent.putExtra("aspectY", 1);
//		// indicate output X and Y
//		cropIntent.putExtra("outputX", 256);
//		cropIntent.putExtra("outputY", 256);
		// retrieve data on return
		cropIntent.putExtra("return-data", true);
		// start the activity - we handle returning in onActivityResult
		startActivityForResult(cropIntent, REQUEST_CROP_IMAGE);
	}

	private void convertImage() {
		String appName = getString(R.string.app_name);
		try {
			File outputImageFile = FileSystemHelper.createSVGFile(appName);
			String outputImagePath = outputImageFile.getAbsolutePath();
			
			//!!!!vladimir: debug
//			if(!WhiteBoardPhotoConverterNativeWrapper.convertImageToSVG(imageUri.getPath(), 
//					outputImagePath, mTesseractDataLocation)) {
//				Log.e(appName, "Internal error: could not convert image");
//				return;
//			}	
			
			//outputImagePath = imageUri.getPath();//!!!!!debug
			showConvertedImage(outputImagePath);
		}
		catch(Exception ex) {
			Log.e(appName, ex.toString());
		}
	}	
	
	private void showConvertedImage(String outputImagePath) {
		Intent svgViewerIntent = new Intent(this, SVGViewerActivity.class);
		svgViewerIntent.putExtra("imagePath", outputImagePath);
		startActivity(svgViewerIntent);
	}
}
