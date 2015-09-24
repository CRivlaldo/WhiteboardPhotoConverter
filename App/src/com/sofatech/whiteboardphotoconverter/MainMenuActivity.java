package com.sofatech.whiteboardphotoconverter;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.text.SimpleDateFormat;
import java.util.Date;

import com.sofatech.whiteboardphotoconverter.util.SystemUiHider;

import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.provider.MediaStore;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;

/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 * 
 * @see SystemUiHider
 */
public class MainMenuActivity extends Activity implements OnClickListener {
	
//	/**
//	 * Whether or not the system UI should be auto-hidden after
//	 * {@link #AUTO_HIDE_DELAY_MILLIS} milliseconds.
//	 */
//	private static final boolean AUTO_HIDE = true;
//
//	/**
//	 * If {@link #AUTO_HIDE} is set, the number of milliseconds to wait after
//	 * user interaction before hiding the system UI.
//	 */
//	private static final int AUTO_HIDE_DELAY_MILLIS = 3000;
//
//	/**
//	 * If set, will toggle the system UI visibility upon interaction. Otherwise,
//	 * will show the system UI visibility upon interaction.
//	 */
//	private static final boolean TOGGLE_ON_CLICK = true;
//
//	/**
//	 * The flags to pass to {@link SystemUiHider#getInstance}.
//	 */
//	private static final int HIDER_FLAGS = SystemUiHider.FLAG_HIDE_NAVIGATION;
//
//	/**
//	 * The instance of the {@link SystemUiHider} for this activity.
//	 */
//	private SystemUiHider mSystemUiHider;
	
	static final int REQUEST_TAKE_PHOTO = 1;
	static final int REQUEST_PICK_IMAGE = 2;
	static final int REQUEST_PICK_IMAGE_ON_KITKAT = 3;
	
	private String mCurrentPhotoPath;
	private String mTessdataLocation;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_mainmenu);

		final View controlsView = findViewById(R.id.mainmenu_content_controls);
		final View contentView = findViewById(R.id.mainmenu_content);

//		// Set up an instance of SystemUiHider to control the system UI for
//		// this activity.
//		mSystemUiHider = SystemUiHider.getInstance(this, contentView,
//				HIDER_FLAGS);
//		mSystemUiHider.setup();
//		mSystemUiHider
//				.setOnVisibilityChangeListener(new SystemUiHider.OnVisibilityChangeListener() {
//					// Cached values.
//					int mControlsHeight;
//					int mShortAnimTime;
//
//					@Override
//					@TargetApi(Build.VERSION_CODES.HONEYCOMB_MR2)
//					public void onVisibilityChange(boolean visible) {
//						if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB_MR2) {
//							// If the ViewPropertyAnimator API is available
//							// (Honeycomb MR2 and later), use it to animate the
//							// in-layout UI controls at the bottom of the
//							// screen.
//							if (mControlsHeight == 0) {
//								mControlsHeight = controlsView.getHeight();
//							}
//							if (mShortAnimTime == 0) {
//								mShortAnimTime = getResources().getInteger(
//										android.R.integer.config_shortAnimTime);
//							}
//							controlsView
//									.animate()
//									.translationY(visible ? 0 : mControlsHeight)
//									.setDuration(mShortAnimTime);
//						} else {
//							// If the ViewPropertyAnimator APIs aren't
//							// available, simply show or hide the in-layout UI
//							// controls.
//							controlsView.setVisibility(visible ? View.VISIBLE
//									: View.GONE);
//						}
//
//						if (visible && AUTO_HIDE) {
//							// Schedule a hide().
//							delayedHide(AUTO_HIDE_DELAY_MILLIS);
//						}
//					}
//				});

		// Set up the user interaction to manually show or hide the system UI.
		contentView.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View view) {
//				if (TOGGLE_ON_CLICK) {
//					mSystemUiHider.toggle();
//				} else {
//					mSystemUiHider.show();
//				}
			}
		});

//		// Upon interacting with UI controls, delay any scheduled hide()
//		// operations to prevent the jarring behavior of controls going away
//		// while interacting with the UI.
//		findViewById(R.id.info_button).setOnTouchListener(
//				mDelayHideTouchListener);
		
		////onInfoButtonClick
		//Intent intent = getIntent();
		final Button buttonInfo = (Button) findViewById(R.id.info_button);
		buttonInfo.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				Intent infoIntent = new Intent(MainMenuActivity.this, InformationActivity.class);
				//myIntent.putExtra("key", value); //Optional parameters
				MainMenuActivity.this.startActivity(infoIntent);
			}});
		
		final Button buttonTakePhoto = (Button) findViewById(R.id.buttonTakePhoto);
		buttonTakePhoto.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
	    		dispatchTakePictureIntent();
			}});
		
		final Button buttonPickPhoto = (Button) findViewById(R.id.buttonPickPhoto);
		buttonPickPhoto.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
	    		dispatchChoosePictureIntent();
			}});	

		mTessdataLocation = Environment.getExternalStorageDirectory() + "/tessdata/";
		prepareAssets();
	}

	@Override
	protected void onPostCreate(Bundle savedInstanceState) {
		super.onPostCreate(savedInstanceState);

		// Trigger the initial hide() shortly after the activity has been
		// created, to briefly hint to the user that UI controls
		// are available.
		delayedHide(100);
	}

	/**
	 * Touch listener to use for in-layout UI controls to delay hiding the
	 * system UI. This is to prevent the jarring behavior of controls going away
	 * while interacting with activity UI.
	 */
//	View.OnTouchListener mDelayHideTouchListener = new View.OnTouchListener() {
//		@Override
//		public boolean onTouch(View view, MotionEvent motionEvent) {
//			if (AUTO_HIDE) {
//				delayedHide(AUTO_HIDE_DELAY_MILLIS);
//			}
//			return false;
//		}
//	};

	Handler mHideHandler = new Handler();
	Runnable mHideRunnable = new Runnable() {
		@Override
		public void run() {
//			mSystemUiHider.hide();
		}
	};

	/**
	 * Schedules a call to hide() in [delay] milliseconds, canceling any
	 * previously scheduled calls.
	 */
	private void delayedHide(int delayMillis) {
		mHideHandler.removeCallbacks(mHideRunnable);
		mHideHandler.postDelayed(mHideRunnable, delayMillis);
	}

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
//		if (requestCode == REQUEST_TAKE_PHOTO && resultCode == RESULT_OK) {
//	        Bundle extras = data.getExtras();
//	        Bitmap imageBitmap = (Bitmap) extras.get("data");
//	        previewImage(imageBitmap);
//	    }
		if(resultCode != RESULT_OK) return;
		
		if (requestCode == REQUEST_TAKE_PHOTO) {
			previewImage(Uri.fromFile(new File(mCurrentPhotoPath)));//!!!!vladimir: move to helper
	    } else if(requestCode == REQUEST_PICK_IMAGE) {
	    	Uri selectedImage = data.getData();
            String[] filePathColumn = {MediaStore.Images.Media.DATA};

            Cursor cursor = getContentResolver().query(selectedImage, filePathColumn, null, null, null);
            cursor.moveToFirst();

            int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
            String filePath = cursor.getString(columnIndex);
            cursor.close();

            previewImage(Uri.fromFile(new File(filePath)));//!!!!vladimir: move to helper
	    } else if(requestCode == REQUEST_PICK_IMAGE_ON_KITKAT) {
	    	 Uri originalUri = data.getData();
	         final int takeFlags = data.getFlags()
	                 & (Intent.FLAG_GRANT_READ_URI_PERMISSION
	                 | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
	         // Check for the freshest data.
	         getContentResolver().takePersistableUriPermission(originalUri, takeFlags);	
	            
	         previewImage(originalUri);    	
	    }
//		InputStream stream = null;
//		if (requestCode == REQUEST_IMAGE_CAPTURE && resultCode == Activity.RESULT_OK) {
//			try {
//				stream = getContentResolver().openInputStream(data.getData());
//				Bitmap bitmap = BitmapFactory.decodeStream(stream);				
//				previewImage(bitmap);
//			} catch (FileNotFoundException e) {
//				e.printStackTrace();
//			} finally {
//				if (stream != null) {
//					try {
//						stream.close();
//					} catch (IOException e) {
//						e.printStackTrace();
//					}
//				}
//			}
//		}
	}
	
	private void previewImage(Uri imageUri) {
		Intent previewIntent = new Intent(MainMenuActivity.this, PreviewPhotoActivity.class);
		previewIntent.putExtra("imageUri", imageUri.toString());
		previewIntent.putExtra("tesseractDataLocation", mTessdataLocation);
		startActivity(previewIntent);
	}
	
	private void dispatchTakePictureIntent() {
    	Log.println(3/*DEBUG*/, "WPC", "dispatchTakePictureIntent");
	    Intent takePictureIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
	    // Ensure that there's a camera activity to handle the intent
	    //if (takePictureIntent.resolveActivity(getPackageManager()) != null) {
	        // Create the File where the photo should go
	        File photoFile = null;
	        try {
	            photoFile = FileSystemHelper.createJPEGFile(getString(R.string.app_name));
	    		mCurrentPhotoPath = photoFile.getAbsolutePath();		
	        } catch (IOException ex) {
	            // Error occurred while creating the File
	            ex.printStackTrace();
	        }
	        // Continue only if the File was successfully created
	        if (photoFile != null) {
	            takePictureIntent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(photoFile));
	            startActivityForResult(takePictureIntent, REQUEST_TAKE_PHOTO);
	        }
	    //}
	}
	
	private void dispatchChoosePictureIntent() {
		if (Build.VERSION.SDK_INT < 19) {
			Intent intent = new Intent();
			intent.setType("image/*");
			intent.setAction(Intent.ACTION_GET_CONTENT);
			startActivityForResult(Intent.createChooser(intent, "Select Picture"), REQUEST_PICK_IMAGE);
		}
		else { //REQUEST_PICK_IMAGE_ON_KITKAT
			Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
		    intent.addCategory(Intent.CATEGORY_OPENABLE);
		    intent.setType("image/jpeg");
		    startActivityForResult(intent, REQUEST_PICK_IMAGE_ON_KITKAT);
		}
	}
	
	private void prepareAssets() {
		String tessdataZip = "tessdata.zip";
		FileSystemHelper.unzipArchiveFromAssets(getAssets(), tessdataZip, mTessdataLocation);
	}
	
//	public String getTesseractDataLocation() {
//		return mTessdataLocation;
//	}
}
