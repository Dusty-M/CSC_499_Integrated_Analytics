# CSC 499: Undergrad Thesis Project
# Integrated Progressive Analytics

## Sample Usage:
### To display results of progressive analysis run the following command from the same directory README.md is located (Note: there is a 2 second delay between each progression to allow the user to read the printout):
`$ bin/CSVParser_tester --filename census_2000_all_places_sample_dimensions.csv`  
`--header SE_T001_001 --header_row_index 1 --first_data_row_index 2`
### To display runtime of analysis of n segments run the following command from the same directory README.md is located:
`$ bin/profiler --filename census_2000_all_places_sample_dimensions.csv`  
`--X_header SE_T003_001 --Y_header SE_T013_001 --num_segs 5 --header_row_index 1 --first_data_row_index 2`

## Status: 
- Basic data ingestion complete (Least Squares Fitting)
- Calculation performed progressively across an arbitrary number of segments
- Summaries have been broken down into 2 stages:
	- ColumnData struct, used to summarize individual data segments
	- LeastSquaresFit class, used to maintain overall summary/projection of calculation
- Profiling can be displayed to terminal via usage of executable 'profiling'

## TODO:
- Calculate confidence intervals.  Ideally, something such as "90% likely bounded between x1 and x2" as well as absolute maximum and minimum bounds.
- Implement visualization of data (results as well as confidence intervals and update provenance)
- Calculate error at each progression and export info to CSV
- Export data from profiler to CSV file(s)
