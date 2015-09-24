package com.sofatech.whiteboardphotoconverter;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import android.content.res.AssetManager;
import android.os.Environment;
import android.util.Log;

public class FileSystemHelper {
	
	public static File getAlbumDir(String appName) {
		File storageDir = null;

		//!!!mount sd
		if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())) {
			
			//storageDir = mAlbumStorageDirFactory.getAlbumStorageDir(getAlbumName());
			storageDir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES);

			if (storageDir != null) {
				if (! storageDir.mkdirs()) {
					if (! storageDir.exists()){
						Log.e(appName, "failed to create directory");
						return null;
					}
				}
			} else {
				Log.e(appName, "failed to get directory");
			}
			
		} else {
			Log.e(appName, "External storage is not mounted READ/WRITE");
		}
		
		return storageDir;
	}
	
	public static File createJPEGFile(String appName) throws IOException {
		// Create an image file name
		String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
		String imageFileName = "JPEG_" + timeStamp + "_";
		File albumF = getAlbumDir(appName);
		Log.d("WPC", albumF.getAbsolutePath());//!!!!vladimir: debug
		File imageF = File.createTempFile(imageFileName, ".jpg", albumF);
		return imageF;
	}
	
	public static File createSVGFile(String appName) throws IOException {
		// Create an image file name
		String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
		String imageFileName = "SVG_" + timeStamp + "_";
		File albumF = getAlbumDir(appName);
		File imageF = File.createTempFile(imageFileName, ".svg", albumF);
		return imageF;
	}
	
	public static Boolean unzipArchiveFromAssets(AssetManager assetManager, String zipFileInAssets, String unzipLocation) {
		try {
			dirChecker(unzipLocation);
			
			InputStream inputStream = assetManager.open(zipFileInAssets); 
			ZipInputStream zipInputStream = new ZipInputStream(inputStream); 
			
			ZipEntry zipEntry = null; 
			while ((zipEntry = zipInputStream.getNextEntry()) != null) { 
				Log.v("WhiteboardPhotoConverter", "Unzipping " + zipEntry.getName()); //!!!!vladimir: debug

				if(zipEntry.isDirectory()) { 
					dirChecker(unzipLocation + zipEntry.getName()); 
				} else {
					if(!unzipEntry(zipInputStream, zipEntry, unzipLocation)) return false;
				} 
			} 
			
			zipInputStream.close(); 
		} catch(Exception e) { 
			Log.e("WhiteboardPhotoConverter", "unzip", e);
			return false;
		} 
		
		return true;
	} 
	
	private static Boolean unzipEntry(ZipInputStream zipInputStream, ZipEntry zipEntry, String unzipLocation) {
		try {
			FileOutputStream outputStream = new FileOutputStream(unzipLocation + zipEntry.getName());
			
			byte[] buffer = new byte[2048];
			BufferedOutputStream bufferedOutputStream = new BufferedOutputStream(outputStream, buffer.length);
	
			int size;
	
			while ((size = zipInputStream.read(buffer, 0, buffer.length)) != -1) {
				bufferedOutputStream.write(buffer, 0, size);
			}
			
			bufferedOutputStream.flush();
			bufferedOutputStream.close();
	
			outputStream.flush();
			outputStream.close();
			
			zipInputStream.closeEntry();
		}
		catch(Exception e) {
			Log.e("WhiteboardPhotoConverter", "unzipEntry", e);
			return false;
		}
		
		return true;
	}
	
	public static String readFileToString(String path) {
		try {
			FileInputStream inputStream = new FileInputStream(path);
			
			byte[] buffer = new byte[inputStream.available()];
			inputStream.read(buffer);
			
			inputStream.close();
			
			return new String(buffer);
		}
		catch(Exception e) {
			Log.e("WhiteboardPhotoConverter", "readFileToString", e);
		}

		return null;
	}
	
//	public static Boolean unzipArchiveFromAssets(AssetManager assetManager, String zipFileInAssets, String unzipLocation) {
//		try {
//			dirChecker(unzipLocation);
//			
//			InputStream inputStream = assetManager.open(zipFileInAssets); 
//			ZipInputStream zipInputStream = new ZipInputStream(inputStream); 
//			
//			ZipEntry zipEntry = null; 
//			while ((zipEntry = zipInputStream.getNextEntry()) != null) { 
//				Log.v("WhiteboardPhotoConverter", "Unzipping " + zipEntry.getName()); //!!!!vladimir: debug
//
//				if(zipEntry.isDirectory()) { 
//					dirChecker(unzipLocation + zipEntry.getName()); 
//				} else { 
//					FileOutputStream outputStream = new FileOutputStream(unzipLocation + zipEntry.getName()); 
//					for (int c = zipInputStream.read(); c != -1; c = zipInputStream.read()) { //!!!!vladimir: slow as I'm at monday morning
//						outputStream.write(c); 
//					} 
//
//					zipInputStream.closeEntry(); 
//					outputStream.close(); 
//				} 
//			} 
//			
//			zipInputStream.close(); 
//		} catch(Exception e) { 
//			Log.e("WhiteboardPhotoConverter", "unzip", e);
//			return false;
//		} 
//		
//		return true;
//	} 

	private static void dirChecker(String dir) {
		File f = new File(dir);
		
		if(!f.isDirectory()) { 
			f.mkdirs(); 
		} 
	} 
}
