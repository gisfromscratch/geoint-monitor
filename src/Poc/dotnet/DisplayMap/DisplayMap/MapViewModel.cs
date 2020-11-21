using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using Esri.ArcGISRuntime.Data;
using Esri.ArcGISRuntime.Mapping;
using Esri.ArcGISRuntime.Portal;
using System.Diagnostics;
using System.Collections.Generic;
using Esri.ArcGISRuntime.Geometry;

namespace DisplayMap
{
    /// <summary>
    /// Provides map data to an application
    /// </summary>
    public class MapViewModel : INotifyPropertyChanged
    {
        public MapViewModel()
        {
            InitAsync();
            Features = new Dictionary<long, Feature>();
        }

        private Dictionary<long, Feature> Features;

        private async void InitAsync()
        {
            var onlinePortal = await ArcGISPortal.CreateAsync();
            var itemId = @"27b0fc32b7954654bf9b7903ae782771";
            var portalItem = await PortalItem.CreateAsync(onlinePortal, itemId);
            Map = new Map(portalItem);
            Map.Loaded += MapLoaded;
        }

        internal async void QueryFeaturesAsync()
        {
            var operationalLayers = Map.OperationalLayers;
            foreach (var layer in operationalLayers)
            {
                var featureLayer = layer as FeatureLayer;
                if (null != featureLayer
                    && 0 == string.Compare("Incidents of Conflict and Protest", featureLayer.Name))
                {
                    // Query the ACLED incidents
                    var queryParams = new QueryParameters();
                    queryParams.WhereClause = @"1=1";

                    var featureTable = featureLayer.FeatureTable;
                    var queryResult = await featureTable.QueryFeaturesAsync(queryParams);
                    var featureCount = 0;
                    foreach (var feature in queryResult)
                    {
                        if (!feature.Geometry.IsEmpty)
                        {
                            featureCount++;
                        }

                        var objectId = (long)feature.Attributes[@"OBJECTID"];
                        if (Features.ContainsKey(objectId))
                        {
                            var oldFeature = Features[objectId];
                            if (!GeometryEngine.Equals(oldFeature.Geometry, feature.Geometry))
                            {
                                Features[objectId] = feature;
                            }
                        }
                        else
                        {
                            Features.Add(objectId, feature);
                        }
                    }
                    Debug.WriteLine(featureCount);
                    Debug.WriteLine(Features.Count);
                }
            }
        }

        private void MapLoaded(object sender, EventArgs e)
        {
            QueryFeaturesAsync();
        }

        private Map _map;

        /// <summary>
        /// Gets or sets the map
        /// </summary>
        public Map Map
        {
            get => _map;
            set { _map = value; OnPropertyChanged(); }
        }

        /// <summary>
        /// Raises the <see cref="MapViewModel.PropertyChanged" /> event
        /// </summary>
        /// <param name="propertyName">The name of the property that has changed</param>
        protected void OnPropertyChanged([CallerMemberName] string propertyName = null) =>
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));

        public event PropertyChangedEventHandler PropertyChanged;
    }
}
