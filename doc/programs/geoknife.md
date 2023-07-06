# sesknife

## Objectives
That program offers services for geometry/geography.

## Examples

```
# Translate two coordinates of the CRS EPSG 3035 into GPS:
geoknife to-gps --srid=3035 "407322.7 2732199.5"

# Translate GPS coordinates (lat long) into ESRI 102030:
geoknife from-gps --srid=102030 "47.993827 -12.30272390"

# Describe the usage:
geoknife --help
```

## Usage

```
geoknife [<options>]  MODE
    Database management and more
  -?,--help
    Shows the usage information., e.g. --help -?
  -l LOG-LEVEL,--log-level=LOG-LEVEL
    Log level: 1=FATAL 2=ERROR 3=WARNING 4=INFO 5=SUMMARY 6=DETAIL 7=FINE 8=DEBUG, e.g. --log-level=123 -l0
  -v,--verbose
    Show more information, e.g. --verbose -v
  MODE
    What should be done:
    to-gps
      Translate a coordinate pair to GPS
geoknife to-gps    Translate a coordinate pair to GPS
  -?,--help
    Shows the usage information., e.g. --help -?
  -S SRID,--srid=SRID
    The identifier of the coordinate system (see POSTGIS)., e.g. --srid=3035
+++ help requested
```
