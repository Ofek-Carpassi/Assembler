# 🔧 Assembler

## Project Grade: UNKNOWN

Welcome to the Assembler repository, the culmination of our efforts in the "Systems Programming Laboratory" (20465) course at the Israel Open University. Crafted with precision in C, this repository houses a sophisticated assembler tailored for a specific assembly language, aimed at emulating the functionality of common system programs.

## 📚 Table of Contents

- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Testing](#testing)
- [Documentation](#documentation)
- [Contributions](#contributions)
- [Written by](#written-by)

## 🏗️ Project Structure

The project boasts a meticulously organized structure, with each component residing in its designated C and header files. This modular approach champions code readability and adherence to strict coding standards. The assembler operates seamlessly through three fundamental stages:

1. **Pre-Assembler**:  
   Sequentially processes the source file, adeptly identifying and cataloging macro definitions. Subsequent macro calls are deftly substituted with these definitions, enhancing program coherence.

2. **First Pass**:  
   Executes a comprehensive scan, diligently identifying labels and constants. Additionally, this pass seamlessly translates all translatable components into binary format, laying the groundwork for the second pass.

3. **Second Pass**:  
   Focuses on resolving components that remained unresolved in the first pass, culminating in their translation into binary format, ensuring the assembly process's completion.

## 🚀 Getting Started

### Prerequisites

Before diving into the realm of assembly, ensure your arsenal includes:

- GCC compiler
- Ubuntu system (Windows compatibility anticipated)

### Installation

1. Clone the Assembler repository:  ```git clone https://github.com/Ofek-Carpassi/Assembler.git```
2. Navigate to the assembler's directory:  ```cd Assembler```
3. Compile the program using the makefile: ```i makefile```

## 💻 Usage

Execute the compiled program using the command:  ```./assembler file1 file2 ... fileN```%
Marvel as it effortlessly generates machine code from the provided assembly files.

## Testing

Dive into the realm of assembly with confidence, armed with several provided assembly language input files. These files not only showcase various operations and data types but also serve as a litmus test for the assembler's robust error handling.

## Documentation

Embark on a journey of understanding with comprehensive documentation accompanying each function in the source code. Delve into the intricacies of the program's functionality through concise header comments, elucidating their purpose and usage. Detailed code comments further enrich your comprehension, ensuring a seamless exploration.

## Contributions

In the realm of this project, contributions are akin to an ancient artifact—cherished but not actively sought.

## Written by

Authored with dedication by Ofek Carpassi and Natanel Sorokin. All rights reserved.
