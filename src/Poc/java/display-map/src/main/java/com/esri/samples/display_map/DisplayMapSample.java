/*
 * Copyright 2017 Esri.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

package com.esri.samples.display_map;

import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.layout.StackPane;
import javafx.stage.Stage;

import java.util.Iterator;
import java.util.List;

import com.esri.arcgisruntime.concurrent.ListenableFuture;
import com.esri.arcgisruntime.data.Feature;
import com.esri.arcgisruntime.data.FeatureQueryResult;
import com.esri.arcgisruntime.data.FeatureTable;
import com.esri.arcgisruntime.data.QueryParameters;
import com.esri.arcgisruntime.layers.FeatureLayer;
import com.esri.arcgisruntime.layers.Layer;
import com.esri.arcgisruntime.mapping.ArcGISMap;
import com.esri.arcgisruntime.mapping.LayerList;
import com.esri.arcgisruntime.mapping.view.MapView;
import com.esri.arcgisruntime.portal.Portal;
import com.esri.arcgisruntime.portal.PortalItem;

public class DisplayMapSample extends Application {

  private MapView mapView;

  @Override
  public void start(Stage stage) {

    try {
      // create stack pane and application scene
      StackPane stackPane = new StackPane();
      Scene scene = new Scene(stackPane);

      // set title, size, and add scene to stage
      stage.setTitle("Display Map Sample");
      stage.setWidth(800);
      stage.setHeight(700);
      stage.setScene(scene);
      stage.show();

      // create a map using a portal item
      String portalUrl = "https://www.arcgis.com";
      Portal onlinePortal = new Portal(portalUrl);
      String portalItemId = "27b0fc32b7954654bf9b7903ae782771";
      PortalItem portalItem = new PortalItem(onlinePortal, portalItemId);
      ArcGISMap map = new ArcGISMap(portalItem);
      map.addDoneLoadingListener(() -> {
        LayerList operationalLayers = map.getOperationalLayers();
        Iterator<Layer> layerIterator = operationalLayers.iterator();
        while (layerIterator.hasNext()) {
          Layer layer = layerIterator.next();
          if (layer instanceof FeatureLayer) {
            FeatureLayer featureLayer = (FeatureLayer)layer;
            if (0 == "Incidents of Conflict and Protest".compareTo(featureLayer.getName())) {
              // Query the ACLED incidents
              QueryParameters queryParams = new QueryParameters();
              queryParams.setWhereClause("1=1");
              
              FeatureTable featureTable = featureLayer.getFeatureTable();
              ListenableFuture<FeatureQueryResult> queryFuture = featureTable.queryFeaturesAsync(queryParams);
              queryFuture.addDoneListener(() -> {
                try {
                  FeatureQueryResult queryResult = queryFuture.get();
                  int featureCount = 0;
                  for (Iterator<Feature> featureIterator = queryResult.iterator(); featureIterator.hasNext();) {
                    Feature feature = featureIterator.next();
                    if (!feature.getGeometry().isEmpty()) {
                      featureCount++;
                    }
                  }
                  System.out.println(featureCount);
                } catch (Exception ex) {
                  ex.printStackTrace();
                }
              });
            }
          }
        }
      });

      // create a map view and set its map
      mapView = new MapView();
      mapView.setMap(map);

      // add the map view to stack pane
      stackPane.getChildren().addAll(mapView);
    } catch (Exception e) {
      // on any error, display the stack trace.
      e.printStackTrace();
    }
  }

  /**
   * Stops and releases all resources used in application.
   */
  @Override
  public void stop() {

    if (mapView != null) {
      mapView.dispose();
    }
  }

  /**
   * Opens and runs application.
   *
   * @param args arguments passed to this application
   */
  public static void main(String[] args) {

    Application.launch(args);
  }

}
