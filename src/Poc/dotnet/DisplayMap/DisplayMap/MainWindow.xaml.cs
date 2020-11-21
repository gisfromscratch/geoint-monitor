using System.Windows;

namespace DisplayMap
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void MapView_GeoViewTapped(object sender, Esri.ArcGISRuntime.UI.Controls.GeoViewInputEventArgs e)
        {
            var mapViewModel = Resources[@"MapViewModel"] as MapViewModel;
            if (null != mapViewModel)
            {
                mapViewModel.QueryFeaturesAsync();
            }
        }
    }
}
