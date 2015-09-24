package com.sofatech.whiteboardphotoconverter;

import java.io.BufferedReader;
import java.io.InputStreamReader;

import com.larvalabs.svgandroid.SVG;
import com.larvalabs.svgandroid.SVGParser;

import android.net.Uri;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.ImageView;
import android.widget.ImageView.ScaleType;

public class SVGViewerActivity extends Activity {
	
	//ToDo:
	// 1) Button "Share"
	// 2) Button "Go to the main menu"
	// 3) SVG Viewer
	
	//private Uri imageUri = null; 

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_svgviewer);
		
		Intent intent = getIntent();
		String imagePath = intent.getExtras().getString("imagePath");
		setPicture(imagePath);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.svgviewer, menu);
		return true;
	}

	private void setPicture(String imagePath) {
		try {
	        SVG svg = readSVGFromFile(imagePath);
	//			BufferedReader reader = null;
	//			try {
	//			    reader = new BufferedReader(
	//			        new InputStreamReader(getAssets().open("android.svg"), "UTF-8")); 
	//
	//			    // do reading, usually loop until end of file reading 
	//			    String mLine = reader.readLine();
	//			    while (mLine != null) {
	//			       //process line
	//			       Log.d("svg", reader.readLine());//!!!vladimir debug 
	//			    }
	//			} catch (Exception e) {
	//				Log.e("svg", e.getMessage());
	//			} finally {
	//			    if (reader != null) {
	//			        reader.close();
	//			    }
	//			}
			
			//SVG svg = SVGParser.getSVGFromAsset(getAssets(), "android.svg");//!!!!vladimir debug
	        //!!!debug
	        {
	        	if(svg.getBounds() != null) Log.d("svg", svg.getBounds().toString());
	        	Log.d("svg", svg.getLimits().toString());
	        	Log.d("svg", "width: " + Integer.toString(svg.getPicture().getWidth()));
	        }
	        
	        ImageView imageView = (ImageView) findViewById(R.id.imageView);	
	        //ImageView imageView = new ImageView(this);
	        imageView.setLayerType(View.LAYER_TYPE_SOFTWARE, null);
	        imageView.setBackgroundColor(Color.RED);//!!!vladimir: background color for the chart can be good option
	        imageView.setImageDrawable(svg.createPictureDrawable());
	        imageView.setScaleType(ScaleType.FIT_XY);
		}
        catch(Exception e) {
        	Log.e("setPicture", e.toString());
        }
	}
	
	private SVG readSVGFromFile(String imagePath) {   
		String svgData = null;
 //       String svgData = FileSystemHelper.readFileToString(imagePath);
        //!!!!!debug
//        {
//        	svgData = "<svg xmlns=\"http://www.w3.org/2000/svg\"\n" +
//			     "xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n" +
//			     "width=\"300\" \n" +
//			     "height=\"300\"\n" +
//			     "viewBox=\"0 0 300 300\">\n" +
//			
//			  "<title>SVG Logo</title>\n" +
//			  "<desc>Designed for the SVG Logo Contest in 2006 by Harvey Rayner, and adopted by W3C in 2009. It is available under the Creative Commons license for those who have an SVG product or who are using SVG on their site.</desc>\n" +
//			 
//			   "<metadata id=\"license\">\n" +
//			     "<rdf:RDF \n" +
//			       "xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n" +
//			       "xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n" +
//			       "xmlns:cc=\"http://web.resource.org/cc/\">\n" +
//			       "<cc:Work rdf:about=\"\">\n" +
//			         "<dc:title>SVG Logo</dc:title>\n" +
//			        " <dc:date>14-08-2009</dc:date>\n" +
//			        " <dc:creator>\n" +
//			        "   <cc:Agent><dc:title>W3C</dc:title></cc:Agent>\n" +
//			        "   <cc:Agent><dc:title>Harvey Rayner, designer</dc:title></cc:Agent>\n" +
//			        " </dc:creator>\n" +
//			        " <dc:description>See document description</dc:description>\n" +
//			        " <cc:license rdf:resource=\"http://creativecommons.org/licenses/by-nc-sa/2.5/\" />\n" +
//			        " <dc:format>image/svg+xml</dc:format>\n" +
//			        " <dc:type rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />\n" +
//			      " </cc:Work>\n" +
//			       "<cc:License rdf:about=\"http://creativecommons.org/licenses/by-nc-sa/2.5/\">\n" +
//			       "  <cc:permits rdf:resource=\"http://web.resource.org/cc/Reproduction\" />\n" +
//			       "  <cc:permits rdf:resource=\"http://web.resource.org/cc/Distribution\" />\n" +
//			       "  <cc:requires rdf:resource=\"http://web.resource.org/cc/Notice\" />\n" +
//			       "  <cc:requires rdf:resource=\"http://web.resource.org/cc/Attribution\" />\n" +
//			       "  <cc:prohibits rdf:resource=\"http://web.resource.org/cc/CommercialUse\" />\n" +
//			       "  <cc:permits rdf:resource=\"http://web.resource.org/cc/DerivativeWorks\" />\n" +
//			       "  <cc:requires rdf:resource=\"http://web.resource.org/cc/ShareAlike\" />\n" +
//			       "</cc:License>\n" +
//			     "</rdf:RDF>\n" +
//			   "</metadata>\n" +
//			   
//			  " <g stroke-width=\"38.0086\" stroke=\"#000\">\n" +
//			   "  <g id=\"svgstar\" transform=\"translate(150, 150)\">\n" +
//			   "    <path id=\"svgbar\" fill=\"#ffb13b\"\n" + 
//			   "      d=\"M-84.1487,-15.8513 a22.4171,22.4171 0 1 0 0,31.7026 h168.2974 a22.4171,22.4171 0 1 0 0,-31.7026 Z\"/>\n" +
//			   "    <use xlink:href=\"#svgbar\" transform=\"rotate(45)\"/>\n" +
//			   "    <use xlink:href=\"#svgbar\" transform=\"rotate(90)\"/>\n" +
//			   "    <use xlink:href=\"#svgbar\" transform=\"rotate(135)\"/>\n" +
//			   "  </g>\n" +
//			   "</g>\n" +
//			   "<use xlink:href=\"#svgstar\"/>\n" +
//			
//			"</svg>";
//        }
        {
        	svgData =
        				"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" +
						"<!-- Generator: Adobe Illustrator 14.0.0, SVG Export Plug-In  -->\n" +
						"<!DOCTYPE svg [\n" +
						"	<!ENTITY ns_flows \"http://ns.adobe.com/Flows/1.0/\">\n" +
						"]>\n" +
						"<svg version=\"1.2\" baseProfile=\"tiny\"\n" +
						"	 xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:a=\"http://ns.adobe.com/AdobeSVGViewerExtensions/3.0/\"\n" +
						"	 x=\"0px\" y=\"0px\" width=\"500px\" height=\"500px\" viewBox=\"0 0 500 500\" xml:space=\"preserve\">\n" +
						"<defs>\n" +
						"</defs>\n" +
						"<rect x=\"0.5\" y=\"0.5\" display=\"none\" fill=\"#FFFFFF\" width=\"499.001\" height=\"499\"/>\n" +
						"</svg>";
        }
        Log.d("WhiteboardPhotoConverter", imagePath + ":\n" + svgData);//!!!vladimir debug
        return SVGParser.getSVGFromString(svgData);
	}
}
