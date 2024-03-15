# 🔧 Assembler

## Project Grade: UNKNOWN

This repository hosts the final project for the "Systems Programming Laboratory" (20465) course at the Israel Open University. It presents a comprehensive assembler for a specific assembly language, crafted in C. The primary objective is to emulate the functionality of common system programs.

## 📚 Table of Contents

- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Written By](#written-by)
- [Usage](#usage)

## 🏗️ Project Structure

The project is structured into multiple C and header files, each fulfilling a distinct purpose within the program. This modular approach aims to enhance code readability, organization, and maintain strict adherence to coding standards. The assembler operates through three principal stages:

1. **Pre-Assembler**:  
   The pre-assembler sequentially processes the source file, identifying macro definitions. Upon detection of a macro, its lines are added to a macro table, and subsequent macro calls are substituted with these lines.

2. **First Pass**:  
   During the first pass, labels and constants are identified. Additionally, the first pass translates all translatable components into binary format.

3. **Second Pass**:  
   The second pass focuses on translating components unresolved in the first pass into binary format.

## 🚀 Getting Started

### Prerequisites

Before running the program, ensure you have the following:

- GCC compiler
- Ubuntu system (Windows compatibility expected)

### Installation

1. Clone the Assembler repository:  ```git clone https://github.com/Ofek-Carpassi/Assembler.git``` 
2. Navigate to the assembler's directory: ```cd Assembler```
3. Compile the program using the makefile: ```i makefile```


## ✍️ Written By

- Natanel Sorokin
- Ofek Carpassi

## 💻 Usage
