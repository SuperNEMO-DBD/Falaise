# Falaise datasets specific classes:

list(APPEND FalaiseLibrary_HEADERS
  datasets/model_id.hpp
  datasets/instance_id.hpp
  datasets/dataset_model.hpp
  datasets/dataset_producer_model.hpp
  datasets/dataset_producer_config.hpp
  datasets/dataset_producer.hpp
  datasets/datachunk.hpp
  datasets/dataset.hpp
  datasets/factory.hpp
  datasets/manager.hpp
  )

list(APPEND FalaiseLibrary_SOURCES
  datasets/model_id.cpp
  datasets/instance_id.cpp
  datasets/dataset_model.cpp
  datasets/dataset_producer_model.cpp
  datasets/dataset_producer_config.cpp
  datasets/dataset_producer.cpp
  datasets/datachunk.cpp
  datasets/dataset.cpp
  datasets/factory.cpp
  datasets/manager.cpp
  )

list(APPEND FalaiseLibrary_TESTS
  datasets/testing/test-datasets.cxx
  )

# - end
