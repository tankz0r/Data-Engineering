## Bonus Project 2
This project is about analyzing taxi rides in New York with Apache Spark.
This project is designed to give you an opportunity to gain experience in programming
systems in the Hadoop ecosystem.  In this case, we use Spark to analyze [taxi rides within New York](https://www1.nyc.gov/site/tlc/about/tlc-trip-record-data.page).
We will use a data set which covers one month.  You will find time and location for each
trip’s start and end.  In the following, this is the data that is meant when we refer to a
trip. The  general  question  is:  Can  we  match  trips  and  return  trips?   For  a  given  trip `a`,  we
consider another trip `b`as a return trip iff   
1.  `b`’s pickup time is within 8 hours after `a`’s dropoff time   
2.  `b`’s pickup location is within r meters of `a`’s dropoff location
3.  `b`’s dropoff location is within r meters of `a`’s pickup location where r is a distance in meters between 50 and 200.
To compute the return trips, you may want to break the problem down into the following series of problems:
1.  Given the (lat,lon) coordinates 
  - a(40.79670715332031,−73.97093963623047)
  - b(40.789649963378906,−73.94803619384766)
  - c(40.73122024536133,−73.9823226928711)
which trips have dropoff locations within r meters of `a`,`b` or `c`?
2.  For each trip a in the dataset, compute the trips that have a pickup location within r meters of `a`’s dropoff location.  These are the return trip candidates.
3.  For all trips `a` in the dataset, compute all trips that may have been return trips for `a`.
Another way to characterize the dataset to be returned would be this pseudo SQL:
```sql
select * from tripsProvided a, tripsProvided b
where
distance(a.dropofflocation , b.pickuplocation) < r and
distance(b.dropofflocation , a.pickuplocation) < r and
a.dropofftime  < b.pickuptime  and
a.dropofftime + 8 hours > b.pickuptime
```
For distance calculations, assume that the earth is a sphere with radius 6371km. Numerical
stability of appropriate formulas is discussed e.g.  at https://en.wikipedia.org/wiki/Great-
circle distance. To complete this project, submit an implementation that manages to compute this query in
less than 10 minutes. The machine used for evaluation will have 64GB RAM and a 6 core Intel i7-3930K processor.

### Prerequisites
1.  Spark: In order to run Spark locally,  please consider the install instructions from
the Spark project:  https://spark.apache.org/docs/2.2.0/.
2.  Sbt: The Scala package manager.  http://www.scala-sbt.org/1.0/docs/Setup.html 
3.  ```spark -shell -i trips.scala```

## Solution
**Result(s):** 423.046
- use bucketss 
- Use as many dimensions for bucketing as possible   
-> Increases dataset size by factor of 3 per 
dimension 
- Use few dimensions for bucketing  
-> Many elements per bucket, large crossproducts 
within buckets 

## Possible improvements:
- Instead of bucketing, sort input and look left and right 
- [z-order curve](https://aws.amazon.com/blogs/database/z-order-indexing-for-multifaceted-queries-in-amazon-dynamodb-part-1/?sc_channel=sm&sc_campaign=zackblog&sc_country=global&sc_geo=global&sc_category=rds&sc_outcome=aware&adbsc=awsdbblog_social_20170517_72417147&adbid=864895517733470208&adbpl=tw&adbpr=66780587)
- Other optimization: [Bloom filter](https://www.kdnuggets.com/2016/08/gentle-introduction-bloom-filter.html)
