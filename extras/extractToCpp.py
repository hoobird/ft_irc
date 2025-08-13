import csv

def extract_rows_with_format():
    rows_with_format = []
    
    with open('numerics.csv', 'r') as file:
        reader = csv.reader(file)
        
        # Skip header
        next(reader)
        
        for row in reader:
            # Check if we have enough columns and Format is not empty
            if len(row) >= 4 and row[3].strip():
                rows_with_format.append(row)
    
    return rows_with_format

def print_rows(rows):
    # Print the results
    for row in rows:
        print(f"Code: {row[0]}, Name: {row[1]}, Origin: {row[2]}, Format: {row[3]}")

def extract_to_cpp_map(rows):
    with open("cppcode.txt", "w") as cpp_file:
        cpp_file.write("std::map<std::string, std::string> numerics;\n")
        for row in rows:
            origin = row[2]
            if not origin.startswith("RFC"):
                continue
            code = row[0]
            name = row[1]
            fformat = row[3]

            cpp_file.write(f"numerics[\"{code}\"] = \"{fformat}\"; // {name}\n")
        cpp_file.write("\n")


def main():
    # Get rows with non-empty Format
    rows = extract_rows_with_format()
    # Print the results
    # print_rows(rows)

    # Extract to C++ map
    extract_to_cpp_map(rows)



if __name__ == "__main__":
    main()


