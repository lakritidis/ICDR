###############################################################################################################
# Required Python modules and libraries
import os.path
import sys
import pandas as pd

# Comment the following line to execute the code from the installed library. Otherwise, Python executes the local files.
sys.path.insert(1, os.path.dirname(sys.path[0]))

import icdr_class


if __name__ == '__main__':
    base_path = ''
    if sys.platform == "linux" or sys.platform == "linux2":
        base_path = '/media/leo/7CE54B377BB9B18B/datasets/EntityResolution/ProductMatching/pricerunner/'
        output_path = '/home/leo/Desktop/dev/Python/FastDynamicRecordLinkage/runs/'
    elif sys.platform == "win32":
        base_path = 'D:/datasets/EntityResolution/ProductMatching/pricerunner/'
        output_path = 'D:/dev/Python/FastDynamicRecordLinkage/runs/'
    else:
        exit(1)

    # entities_file = base_path + 'coffee_makers_2.csv'
    entities_file = base_path + 'tableB_large.csv'

    input_dataframe = pd.read_csv(entities_file)
    input_dataframe.head(10)

    index = icdr_class.icdr()
    #index.build(input_file=entities_file, lex_size=2097152, min_term_length=1, max_term_length=100, block_size=128)
    #index.write("output/")
    index.read("output/")
    # index.display_index()
    # index.display_records()
    # index.display_entities()
    # index.compute_stats(1)

    #records = index.retrieve_relevant(q="bosch coffee maker", num_results=20)
    #print("Results relevant to q:\n", records)

    #recs = index.get_records()
    #print(recs.iloc[9, :])

    negatives = index.retrieve_negative(rid=10, num_results=20)
    print("Negative samples:\n", negatives)


    #index.display_index()
    #index.write(output_path)

    index.destroy()

    #index2 = icdr()
    #index2.read(output_path)
    #index2.display_index()
    #index2.display_entities()
    #index2.destroy()
