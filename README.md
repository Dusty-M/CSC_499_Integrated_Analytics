# CSC 499: Undergrad Thesis Project
# Integrated Progressive Analytics

## Sample Usage:
`$ CSVParser\_tester --filename census\_2000\_all\_places\_sample\_dimensions.csv`  
`--header SE\_T001\_001 --header\_row\_index 1 --first\_data\_row\_index 2`
## Status: 
- Basic data ingestion complete (Least Squares Fitting)
- Calculation performed progressively across an arbitrary number of segments
- Summaries have been broken down into 2 stages:
	- ColumnData struct, used to summarize individual data segments
	- LeastSquaresFit class, used to maintain overall summary/projection of calculation

## TODO:
- Calculate confidence intervals.  Ideally, something such as "90% likely bounded between x1 and x2" as well as absolute maximum and minimum bounds.
- Implement profiling, measure times required to perform analysis in progressively or as a batch process.
- Implement visualization of data (results as well as confidence intervals and update provenance)
