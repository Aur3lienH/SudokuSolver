# SUDOKU MASTER

## Description

This program is a sudoku solver that takes a grid image, extract the digits, and solves the grid. To achieve that, we need to go through different steps :
- A preprocessing of the image to make it easier to work with, for that, it will apply different filters:
  - Grayscale
  - Contrast
  - Mediam
  - Smoothing
- A treatment of the image to extract the cells of the grid, again for that, it will go through different steps:
  - Line detection
  - Square Detection
  - Perspective shifting
  - Extract cells
- A neural network that will take every cell and recognize the digit in it, here the work around the network and the method used was important:
  - Creating a Network (Fully Connected Network)
  - Backpropagation
    - Loss function
    - Backpropagation across Fully Connected Layer
    - Activation Function
  - Overfitting 
    - L2 Regularization
    - Dropout Layer
    - Data augmentation
  - Matrix operations optimization
    - SMID instructions
    - Multithreading
    - Block matrix operations
- A solving algorithm that will finally solve the Sudoku, it uses a backpropagation algorithm

## How to use

### Installation

To install the project, you need to clone the repository and then compile it.

```git clone git@github.com:Aur3lienH/SudokuSolver.git ```

#### Compilation

Prerequisites : make, gcc, gdk, sdl2, sld2-image, gtk3

To compile the project, run the following command in the root directory of the project:

```cmake -S . -B build```

```cmake --build build```

### Execution

To enable the execution of the program, you need to give the execution rights to the executable. 

```chmod +x build/solver```

The executable is located in the build/ directory and is named solver. So to run it, in the root directory : 

```./build/solver```

## Usage

### Basic usage 

Train the network with the Mnist dataset :
    
```./build/solver -t```

Run the tests :

```./build/solver --tests```


The program has several flags for differents use and for testing and debugging purposes. Here is the list of all the flags : 

* --train or -t : train the neural network with the Mnist dataset (1 training dataset and 1 testing dataset)

* --tests : run the tests

* --ui : run the program with the user interface

### Usage with the UI

The first window that appears will let you chose between the solver and the training part.

#### Solving part

Here, you will be able to load an image, process it, extract the digits and then finally solve the grid.
Every step will be visible on the right of the window.

#### Training part

In this part, you can train and/or load a digit recognition model.
After that, you will be able to test it as well as use the Drawdigit function that will display a new window and draw digits that the model will try to recognize.