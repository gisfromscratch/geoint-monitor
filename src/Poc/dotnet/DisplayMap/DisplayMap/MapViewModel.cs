using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;
using Esri.ArcGISRuntime.Data;
using Esri.ArcGISRuntime.Geometry;
using Esri.ArcGISRuntime.Location;
using Esri.ArcGISRuntime.Mapping;
using Esri.ArcGISRuntime.Security;
using Esri.ArcGISRuntime.Symbology;
using Esri.ArcGISRuntime.Tasks;
using Esri.ArcGISRuntime.UI;
using Esri.ArcGISRuntime.Portal;
using System.Diagnostics;

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
        }

        private async void InitAsync()
        {
            var onlinePortal = await ArcGISPortal.CreateAsync();
            var itemId = @"27b0fc32b7954654bf9b7903ae782771";
            var portalItem = await PortalItem.CreateAsync(onlinePortal, itemId);
            Map = new Map(portalItem);
            Map.Loaded += MapLoaded;
        }

        private async void MapLoaded(object sender, EventArgs e)
        {
            var operationalLayers = Map.OperationalLayers;
            foreach (var layer in operationalLayers)
            {
                var featureLayer = layer as FeatureLayer;
                if (0 == string.Compare("Incidents of Conflict and Protest", featureLayer.Name))
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
                    }
                    Debug.WriteLine(featureCount);
                }
            }
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
