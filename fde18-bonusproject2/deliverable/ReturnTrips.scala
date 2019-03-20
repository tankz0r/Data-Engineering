import org.apache.spark.sql.SparkSession
import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.SparkConf
import org.apache.spark.sql.types._
import org.apache.spark.sql.functions._
import org.apache.spark.sql.Column
import org.apache.spark.sql.Dataset
import org.apache.spark.sql.Row
import org.apache.spark.sql.functions.{lit}

object ReturnTrips {

  def distance(startLat: Column, startLon: Column, endLat: Column, endLon: Column): Column = {
    val dLat = endLat - startLat
    val dLon = endLon - startLon
    val lat1 = startLat
    val lat2 = endLat
    val a = pow(sin(dLat/2), 2) + pow(sin(dLon/2), 2) * cos(lat1) * cos(lat2)
    val c = lit(2) * atan2(sqrt(a), sqrt(lit(1)-a))
    lit(6371.0) * c * 1000
  }

  def compute(trips : Dataset[Row], dist : Double, spark : SparkSession) : Dataset[Row] = {
    import spark.implicits._
    val trips_ = trips
      .withColumn("pickup_longitude", toRadians($"pickup_longitude"))
      .withColumn("pickup_latitude", toRadians($"pickup_latitude"))
      .withColumn("dropoff_longitude", toRadians($"dropoff_longitude"))
      .withColumn("dropoff_latitude", toRadians($"dropoff_latitude"))
      .withColumn("tpep_pickup_datetime", unix_timestamp($"tpep_pickup_datetime"))
      .withColumn("tpep_dropoff_datetime", unix_timestamp($"tpep_dropoff_datetime")).cache()

    val max_pickup_lat = trips_.agg(abs(max($"pickup_latitude"))).first.getDouble(0)
    val max_dropoff_lat = trips_.agg(abs(max($"dropoff_latitude"))).first.getDouble(0)
    val max_lat_rad = math.max(max_pickup_lat, max_dropoff_lat)
    val max_lat_r = math.sin(math.Pi/2-max_lat_rad) * (6371.0 * 1000)
    val dlong = dist / max_lat_r
    val dlat = dist / (6371.0 * 1000)

    val trips_buck = trips_
      .withColumn("pick_lon_buck", floor($"pickup_longitude" / dlong))
      .withColumn("drop_lon_buck", floor($"dropoff_longitude"/ dlong))
      .withColumn("pick_lat_buck", floor($"pickup_latitude" / dlat))
      .withColumn("drop_lat_buck", floor($"dropoff_latitude"/ dlat))
      .withColumn("pick_time_buck", floor($"tpep_pickup_datetime" / 28800))
      .withColumn("drop_time_buck", floor($"tpep_dropoff_datetime" / 28800))
    val trips_buck_neigh = trips_buck
      .withColumn("pick_lon_buck", explode(array($"pick_lon_buck"-1, $"pick_lon_buck", $"pick_lon_buck"+1)))
      .withColumn("drop_lon_buck", explode(array($"drop_lon_buck"-1, $"drop_lon_buck", $"drop_lon_buck"+1)))
      //.withColumn("pick_lat_buck", explode(array($"pick_lat_buck"-1, $"pick_lat_buck", $"pick_lat_buck"+1)))
      //.withColumn("drop_lat_buck", explode(array($"drop_lat_buck"-1, $"drop_lat_buck", $"drop_lat_buck"+1)))
      .withColumn("pick_time_buck", explode(array($"pick_time_buck"-1, $"pick_time_buck", $"pick_time_buck"+1)))
//      .withColumn("drop_time_buck", explode(array($"drop_time_buck"-1, $"drop_time_buck", $"drop_time_buck"+1)))

    val result = trips_buck.as("a").join(trips_buck_neigh.as("b"),
          ($"a.drop_lon_buck" === $"b.pick_lon_buck") &&
          ($"a.pick_lon_buck" === $"b.drop_lon_buck") &&
        //  ($"a.drop_lat_buck" === $"b.pick_lat_buck") &&
        //  ($"a.pick_lat_buck" === $"b.drop_lat_buck") &&
          ($"a.drop_time_buck" === $"b.pick_time_buck") &&
          ($"a.tpep_dropoff_datetime" < $"b.tpep_pickup_datetime") &&
          ($"a.tpep_dropoff_datetime" + 28800 > $"b.tpep_pickup_datetime") &&
          (distance($"a.pickup_latitude", $"a.pickup_longitude", $"b.dropoff_latitude", $"b.dropoff_longitude") < dist) &&
          (distance($"a.dropoff_latitude", $"a.dropoff_longitude", $"b.pickup_latitude", $"b.pickup_longitude") < dist))

    result

}
}
