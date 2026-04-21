# ECE 312: Lab 5 – 20 Questions: Guess the Animal

**Course:** ECE 312H – Software Design and Implementation I (Honors)

**Instructor:** Pedro Santacruz

**Semester:** Spring 2026

---

## Overview

In this lab, you will implement a learning guessing game that uses a binary decision tree to guess animals.
The program asks yes/no questions, makes guesses, and learns from its mistakes by dynamically growing the tree.
You'll implement iterative tree traversal, hash-based indexing, and undo/redo functionality.

---

## Learning Objectives

By completing this lab, you will:

- Implement and traverse binary decision trees
- Use explicit stacks for iterative traversal (avoiding recursion)
- Implement queue-based BFS (breadth-first search) for tree serialization
- Design and implement a hash table with collision handling
- Implement undo/redo functionality with dual stacks
- Build an interactive terminal application with `ncurses`
- Practice comprehensive memory management

---

## The Lab Workflow

Follow the same four-step process from previous labs:

### Step 1: Develop Your Solution

Write and test your code locally using VS Code. Ensure you compile **all** `.c` files together. VS Code's default C/C++ configuration should handle this automatically when you click "Run" or use the play button.

### Step 2: Test Your Solution

Use the provided test file and write your own test cases to verify correctness. Test edge cases like:

- Empty tree operations
- Root node replacement during learning
- Undo/redo on empty stacks
- Hash table collisions
- Tree integrity violations

### Step 3: Submit to Gradescope

Upload your `ds.c`, `game.c`, and `utils.c` files to Gradescope and review the autograder feedback.

### Step 4: Iterate Until Complete

Fix issues based on feedback and resubmit as needed.

---

## Important Reminders

- **Hidden test cases:** You will NOT have access to all test cases. The autograder includes additional tests for edge cases.
- **Think critically** about boundary conditions and memory management
- **Write your own tests** beyond what is provided
- **Test thoroughly** before submitting
- **Memory leaks:** Your code must not leak any memory—test with Valgrind if available
- **No recursion:** Tree traversal must use explicit queues, not recursive calls

---

## Setup

This Programming Assignment utilizes an external library: `ncurses`.

As such, you will need to modify `task.json` to tell `gcc` to link the library during compilation.
We will accomplish this by following the appropriate steps depending on your platform:

### Windows
1. Open `MSYS2 UCRT64` application. It should open a terminal.
2. Type `pacman -S mingw-w64-ucrt-x86_64-ncurses`.
3. Follow all prompts to install.
4. After, run `pacman -Q` and verify `mingw-w64-ucrt-x86_64-ncurses` is shown in the output.
5. Open File Explorer and go to the path `C:\msys64\ucrt64\include\ncursesw` or wherever you have msys64 installed.
6. You should see a bunch of header files. This is where MSYS2 downloaded the `ncurses` header files.
7. Now, you will modify `task.json` to include arguments telling `gcc` where these header files exist and what library to use.
   Follow the example below.
8. Run your code to verify it compiles.

```json
"args": [
"-fdiagnostics-color=always",
"-g",
"${fileDirname}\\*.c",
"-o",
"${fileDirname}\\${fileBasenameNoExtension}.exe",
"-IC:\\msys64\\ucrt64\\include\\ncursesw",
"-lncursesw"
],
```

**Optional**

VSCode's C/C++ linter we installed may not know about the `ncurses` library.
If it doesn't you will have the red swiggly lines showing that indicate an error at the `include <ncurses.h>`.
This will not cause an error when compiling because we told `gcc` about the library through `task.json`.

To do the same for the linter, removing the fake error, do the following:
1. `Ctrl+Shift+P` to run a command on VSCode.
2. Type `C/C++: edit Configurations (JSON)`. This should create a `c_cpp_properties.json` in `.vscode` folder.
3. Replace whatever is in the file with below. Note, please update the `ncurses` library path if it's not installed in the deafult location. We are simply adding the same path we specified eariler in the `task.json`.

```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "C:\\msys64\\ucrt64\\include\\ncursesw"
            ],
            "cStandard": "c99",
            "compilerPath": "C:/msys64/ucrt64/bin/gcc.exe"
        }
    ],
    "version": 4
}
```

**Mac**

1. Open up a terminal and type `brew install ncurses`.  If homebrew is not installed in your machine, go ahead and do that first.
2. Take note of the folder where things were installed.  After installation part of the printout should read something like this:
```
For compilers to find ncurses you may need to set:
  export LDFLAGS="-L/opt/homebrew/opt/ncurses/lib"
  export CPPFLAGS="-I/opt/homebrew/opt/ncurses/include"
```
You will need the text in quotes later.

3. Open up `tasks.json` on your project and add
```
        "-I/opt/homebrew/opt/ncurses/include",
        "-L/opt/homebrew/opt/ncurses/lib",
        "-lncurses",
        "-lpanel"
``` 
to you `args` section.  Your `args` section should look something like this:
```
    "args": [
                "-fdiagnostics-color=always",
                "-g",
                "*.c",
                "-o",
                "${fileDirname}/${fileBasenameNoExtension}",
                "-I/opt/homebrew/opt/ncurses/include",
                "-L/opt/homebrew/opt/ncurses/lib",
                "-lncurses",
                "-lpanel"
```

4. Run your code to verify it compiles.
5. In Mac, you have to open up a new terminal ``Ctrl + Shift + ` ``  and run manually using `./main`.


---

## Game Description

### How It Works

The program plays a guessing game where it tries to identify an animal you're thinking of by asking yes/no questions. When it guesses incorrectly, it learns by asking you for:
-   The correct animal
-   A distinguishing question
-   The answer to that question for your animal

The tree expands with each wrong guess, allowing the program to become smarter over time. You can save and load the tree to preserve learned knowledge between sessions.

### Example Session

```
Think of an animal...
Is it a mammal? [y/n] y
Does it have stripes? [y/n] n
Is it an dog? [y/n] n

I got it wrong! What animal were you thinking of?
> Cat

What yes/no question distinguishes a giraffe from an elephant?
> Does it meow?

What is the answer for cat? [y/n] y

Thanks! I'll remember that.
```

---

## Assignment Description

You will implement a complete 20 Questions game with advanced features. The assignment is divided into multiple components.

### Files Provided

| File        | Description                                                  |
|-------------|--------------------------------------------------------------|
| `lab5.h`    | Header file with all data structures and function prototypes |
| `main.c`    | Main program with ncurses GUI (mostly provided)              |
| `ds.c`      | Data structures: node, stack, hash table                     |
| `game.c`    | Gameplay logic with iterative tree traversal                 |
| `persist.c` | Save/load functionality                                      |
| `utils.c`   | Utility functions                                            |
| `tests.c`   | Unit test suite                                              |

---

## Part 1: Data Structures

You will implement core data structures needed for the game in `ds.c`.

All structs mentioned below are defined in `lab5.h`

### 1.1 Node Functions

Basic tree node creation and management.

---

#### Node Structure

```c
typedef struct Node {
    char *text; // Points to an array of chars (string)
    struct Node *yes; // Points to a single Node
    struct Node *no; // Points to a single Node
    int isQuestion; // 1 = question, 0 = animal
} Node;
```

---

#### Functions to Implement

```c
Node *create_question_node(const char *text);
Node *create_animal_node(const char *text);
void free_tree(Node *node);
void free_node(Node *node);
int count_nodes(Node *node);
```

**`create_question_node`**

**Description:** Allocates and initializes a question node using Node struct.
   
**Parameters:**
- `question` – text of the yes/no question

**Returns:**
- Pointer to newly allocated question Node

**Behavior:**
- Allocate memory for a Node structure
- Set `isQuestion` to 1 (true)
- Return the new node

---

**`create_animal_node`**

**Description:** Allocates and initializes an animal node using Node struct.

**Parameters:**
- `animal` – name of the animal

**Returns:**
- Pointer to newly allocated animal Node

**Behavior:**
- Allocate memory for a Node structure
- Set `isQuestion` to 0 (false)
- Return the new node

---

**`free_tree`**

**Description:** Recursively frees all nodes in the tree.

**Parameters:**
- `node` – root of tree/subtree to free

**Behavior:**
- Free all Nodes that full under the provided root Node

**HINT:** Free children before freeing the parent.

**Note:** This is one of the few recursive functions allowed in this assignment.

---

**`free_node`**

**Description:** Frees node and it's immediate resources.

**Parameters:**
- `node` – the Node to free

**Behavior:**
- Free the Node's immediate resources (`text`)
- Free the Node

---

**`count_nodes`**

**Description:** Counts the total number of nodes in the tree.

**Parameters:**
- `root` – root of tree to count

**Returns:**
- Total number of nodes

**Behavior:**
- Returns the number of Nodes that fall under the provided root Node.

**Note:** This is one of the few recursive functions allowed in this assignment.

---

### 1.2 EditStack

The edit stack stores edit records for undo/redo functionality.

---

#### EditStack and Edit Structure

```c
typedef struct Edit {
    Node *oldQuestion; // Old Question (parent) Node
    Node* newQuestion; // New Question (parent) Node
    Node *oldAnimal; // Old Animal (child) Node
    Node *newAnimal; // New Animal (child) Node
	EntryInfo newAnimalInfo; // Info about the new Animal Node
    int wasYesChild; // (1) if OldAnimal was yes child, (0) if no child, (-1) if root
} Edit;

typedef struct EditStack {
    Edit *edits; // Points to an array of Edits
    int size; // How many Edits are currently in the above array
    int capacity; // How many Edits are currently allocated for the above array
} EditStack;
```

---

#### Functions to Implement

```c
void es_init(EditStack* stack);
void es_push(EditStack* stack, Edit edit);
Edit es_pop(EditStack* stack);
int es_is_empty(EditStack* stack);
void es_redo_clear(EditStack *stack, Hash *hash);
void es_free(EditStack* stack);
```

**`es_init`**

**Description:** Initializes an empty EditStack.

**Parameters:**
- `stack` – pointer to EditStack to initialize

**Behavior:**
- Initialize EditStack with capacity 16
- Update all/ allocate variables of the struct as neccesary.

---

**`es_push`**

**Description:** Pushes an edit record onto the stack.

**Parameters:**
- `stack` – pointer to EditStack
- `edit` – Edit struck to push

**Behavior:**
- Add edit to array and increment size
- Store the Edit, `edit`, to the top of `stack->edits[]`
    - If you run out of space, please increment      by `*2`
- Modify/Update EditStack, `stack`, as needed

---

**`es_pop`**

**Description:** Removes and returns the top Edit from the EditStack.

**Parameters:**
- `stack` – pointer to EditStack

**Returns:**
- Edit struck

**Behavior:**
- Return Edit at top of `stack->edits[]`
- You do not need to update the EditStack's capacity

---

**`es_is_empty`**

**Description:** Checks if the EditStack is empty.

**Parameters:**
- `stack` – pointer to EditStack

**Returns:**
- 1 if empty, otherwise 0

---

**`es_redo_clear`**

**Description:** Clears a Redo EditStack by freeing ONLY relevant memory.

**Parameters:**
- `stack` – pointer to Redo EditStack (an EditStack used for redo purposes)
- `hash` - pointer to hash table for animal nodes

**Behavior:**
- For all Edits in the EditStack `stack`, free only the elements that are guaranteed to not be needed in the game/ current tree.
    - HINT: We need to free Nodes with `free_node()`, but which ones?
    - Also, be sure to remove the associated entries in `hash` (this might make more sense when you understand the game logic)
- Set size to 0
- DO NOT free the EditStack or any of its immediate elements (ex. `stack->edits`)

**Note:** Improper implementation of this function may result in double frees.

--- 

**`es_free`**

**Description:** Free an EditStack by freeing ONLY immediate memory.

**Parameters:**
- `stack` – pointer to Redo EditStack (an EditStack used for redo purposes)

**Behavior:**
- Free all immediate memory elements of `stack`
- DO NOT free the EditStack itself

**Note:** Improper implementation of this function may result in double frees.

---

### 1.3 Hash Table

The hash table provides fast lookup from canonicalized keys to stats about animal nodes actively in the tree.

---

#### Entry and Hash Structure

```c
typedef struct EntryInfo{
    int timesGuessed; // How many times the node was guessed
    int timesCorrect; // How many times the node was guseed correctly
} EntryInfo;

typedef struct Entry {
    char *key; // An array of chars (string)
    EntryInfo info; // Extra information about Entry
    struct Entry *next; // Pointer to next associated Entry
} Entry;

typedef struct Hash {
    Entry **buckets; // An array of Entry Chains
    int size; // Number of Entrys
    int nbuckets; // Number of buckets (Entry*) allocated for above array
} Hash;
```

---

#### Functions to Implement

```c
char *canonicalize(const char *text);
void h_init(Hash *h, int num_buckets);
int h_put(Hash *h, const char *key, EntryInfo info); 
int h_contains(const Hash *h, const char *key);
Entry *h_get_entry(Hash *h, const char* key);
int h_remove(Hash *h, const char *key);
void  h_free(Hash *h);
```

**`canonicalize`**

**Description:** Converts a string to canonical form for hashing.

**Parameters:**
- `s` – input `char*` to canonicalize

**Returns:**
- Newly allocated canonicalized `char*`

**Behavior:**
- Convert a string to canonical form:
- Convert to lowercase
- Keep only alphanumeric characters (`isalnum()`)
- Replace spaces with underscores (`isspace()`)
    - You do not have to worry about leading/trailing whitespace
- Remove punctuation

**Example:**
`"Does it meow?"` → `"does_it_meow"`

---

**`h_init`**

**Description:** Initializes an empty hash table.

**Parameters:**
- `h` – pointer to a Hash
- `nbuckets` – number of buckets

**Behavior:**
- Allocate buckets array using `calloc()` (initializes everything to 0)
- Set remaining fields.
- You can assume Hash is allocated for you.

---

**`h_contains`**

**Description:** Checks if the hash table contains the given key.

**Parameters:**
- `h` – pointer to Hash
- `key` – canonicalized key char* (string)

**Returns:**
- 1 if found, 0 otherwise

**Behavior:**
- Search the Entry chain at `h->buckets[idx]` for an Entry with a matching key
- If an Entry is found, return 1 , else 0

---

**`h_put`**

**Description:** Adds an new key (*animal*) to the hash table.

**Parameters:**
- `h` – pointer to Hash
- `key` – canonicalized key char* (string)

**Returns:**
- 1 if value was added, 0 if it already existed

**Behavior:**
- Check if an entry already exist, return 0 if thats the case
- If an entry for the key does not exist:
    - Create (allocate and initialize) a new Entry.
    - Insert new Entry at head of chain (`buckets[idx]`).
    - Return 1
- You do not need ,to reallocate Hash, `h`.

---

**`h_get_entry`**

**Description:** Returns pointer to the Entry for the given key.

**Parameters:**
- `h` – pointer to Hash
- `key` – canonicalized key string

**Returns:**
- Pointer to Entry, or NULL if key not found

**Behavior:**
- Search Entry chain at `h->buckets[idx]` for an Entry with a matching key
- If found, return pointer, else `NULL`

---

**`h_remove`**

**Description:** Remove a key from Hash.

**Parameters:**
- `h` – pointer to Hash
- `key` – canonicalized key string

**Returns:**
- 1 if successful, else 0

**Behavior:**
- Search Entry chain at `h->buckets[idx]` for an Entry with a matching key
- If found, remove Entry from the Entry chain
    - `free()` the associated memory
    - Update other Hash, `h`, variables as neccesary
    - return 1
- If entry not found, return 0

---

**`h_free`**

**Description:** Frees all memory associated with the hash table.

**Parameters:**
- `h` – pointer to Hash

**Behavior:**
- Free all Entry chains in `buckets[]`
    - Free all memory associated with each Entry in the chain
- Free all memory associated with the Hash
- Set buckets to NULL and size to 0
- Do not free the Hash itself.

---

## Part 2: Game Logic

You will implement the gameplay mechanics using **iterative traversal**.

### 2.1 Play Game Function

#### Function to Implement

```c
void play_game(void);
```

**Description:** Conducts one round of the guessing game using iterative tree traversal.

**Global Parameters from main.c:**
- `g_root` – pointer to root node
- `g_index` - hash table for animal nodes

**Behavior:**

The function must use the FrameStack to traverse the tree without recursion. The general behavior is:

- Initialize tracking variables
- While current tracking Node is not empty:
    - If current node is a **question**:
        - Present the question to the user with helper functions from main (`display_message()`)
        - Get the user's answer with `get_yes_no()` from main.c
        - Update the current node with the appropriate child (yes or no)
    - If current node is an **animal**:
        - Make a guess to the user
            - "Is it a(n) [animal]? (y/n)"
        - Get the user's answer with `get_yes_no()` from main.c
        - If correct: display congratulations, animal stats, and exit loop
            - Recall animal stats are in `g_index` global hash table
        - If wrong: call `learnNewAnimal()` function with the appropriate tracking variables and exit loop.

**Requirements:**
- Do NOT use recursion for tree traversal
- Handle NULL root (empty tree) gracefully
- Track parent node and which branch was taken for `learnNewAnimal()`
- Use ncurses helper functions from main. **DO NOT** call ncurses functions directly.
- Use provided helper functions: `get_yes_no()` and `get_input()`
- Call `learnNewAnimal()` when guess is wrong
- Update the guessed animal stats in `g_index` as appropriate.

**String Buffers**

We recommend using buffers to present/build questions. We already declared a buffer for you called `prompt`. To use the buffers please follow the example code below and in the file:

```c
char prompt[512];
snprintf(prompt, sizeof(prompt), "%s (y/n): ", node->text);
int answer = get_yes_no(row, 2, prompt);
```

---

### 2.2 Learn New Animal Function

#### Function to Implement

```c
void learnNewAnimal(Node* wrongAnimal, Node* parentQuestion, int parentAnswer);
```

**Description:** Updates the tree when the program guesses an animal incorrectly.

**Parameters:**
- `wrongAnimal` – the animal Node that was guessed incorrectly
- `parentQuestion` – parent of the wrongAnimal Node (NULL if wrongGuess is root)
- `parentAnswer` – which child pointer to update (1 = yesAnswer child, 0 = noAnswer child)

**Global Parameters from main.c:**
- `g_undo` – stack for undo operations
- `g_redo` – stack for redo operations
- `g_index` - hash table for animal nodes

**Behavior:**
- Get the correct animal, distinguishing question, and associated answer from the user using `get_input()` from main.c
    - Use the default buffers declared to create/ store questions/ answers. An example is provided for you.
    - **IMPORTANT** The user is *NOT* guaranteed to return an a new animal! It is your job check if the user returned an animal already in the tree, and prompt them to enter another animal until they specify a new animal not in the tree.
- Create new Question and Animal Nodes (lets call them `newQ` and `newA`)
- Update the `g_index` by putting the new canonicalized animal with `h_put()`
- Link the Question Node's children based on the user's answer to their question
    - If they said **no**, `newQ->no = newA`
    - Else if they said **yes**, `newQ->yes = newA`
    - The other child for `newQ` is the `wrongAnimal` Node.
- Create a new Edit and initialize its variables appropriately
    - If `parentQuestion` is NULL, set `edit.oldQuestion` = NULL and `edit.wasYesChild` = -1
    - Else, set `edit.oldQuestion` == `parentQuestion` and update `parentQuestion` `yes` or `no` variable based on
      `parentAnswer`.
    - The `newAnimalInfo` can be initialized to 0 for both stats.
- Push the new Edit onto `g_undo` and clear `g_redo` with `es_redo_clear()`
- Display a thanks to the user

**Requirements:**
- Allocate memory for new nodes using `create_question_node()` and `create_animal_node()`
- Handle the special case when wrong guess is root (`parent == NULL`)
- Properly link parent-child relationships based on answer
- Record the edit for undo functionality
- Clear redo stack after new edit
- Update the Hash index with the new animal
- Display success message to user

**Example Tree Modification:**

Before learning (wrong guess was "elephant"):
```
    dog (root)
```

After learning (user said "giraffe", question is "Does it have a long neck?", answer is "yes"):
```
         Does it meow?
       /              \
     cat             dog
```

---

### 2.3 Undo Function

#### Function to Implement

```c
int undo_last_edit(void);
```

**Description:** Undoes the most recent tree modification.

**Global Parameters from main.c:**
- `g_undo` – stack for undo operations
- `g_redo` – stack for redo operations
- `g_index` - hash table for animal nodes

**Returns:**
- 1 if undo was successful
- 0 if undo stack was empty (nothing to undo)

**Behavior:**
- Pop most recent Edit from `g_undo`
- Restore the Edit structure based on it's `wasYesChild` variable
    - You are undoing what you did in `learnNewAnimal` when creating the Edit
- Update the edit's `newAnimalInfo` to contain the most up-to-date stats
- Remove the `newAnimal` from the `g_index` as it's no longer in the tree.
- Push the restored Edit to `g_redo`

**Requirements:**
- Correctly restore parent-child relationships
- Handle root replacement case (`parent == NULL`)
- Do not free Nodes that might be redone

---

### 2.4 Redo Function

#### Function to Implement

```c
int redo_last_edit(void);
```

**Description:** Redoes a previously undone edit.

**Global Parameters from main.c:**
- `g_undo` – stack for undo operations
- `g_redo` – stack for redo operations
- `g_index` - hash table for animal nodes

**Returns:**
- 1 if redo was successful
- 0 if redo stack was empty (nothing to redo)

**Behavior:**
- Pop most recent Edit from `g_redo`
- Reapply the Edit structure based on it's `wasYesChild` variable
    - You are redoing what you did in `learnNewAnimal` when creating the Edit
- Add the `newAnimal` to `g_index` as it's now back in the tree.
- Push the restored Edit to `g_undo`

**Requirements:**
- Correctly restore parent-child relationships
- Handle root replacement case (`parent == NULL`)

---

### 2.6 Helper Functions (Provided)

The following helper functions are provided in `main.c`:

**`get_yes_no(int row, int col, const char* prompt)`**
- Displays a prompt and gets y/n input from user
- Returns 1 for yes, 0 for no
- Handles invalid input by re-prompting

**`get_input(int row, int col, const char* prompt)`**
- Displays a prompt and gets string input from user
- Returns pointer to input string
- Allocates memory for the string (caller should copy if needed)

**display_header_play_game()**
- Display the header for when playing a game

**display_message(int y, int x, const char *prompt)**
- Display a message

**display_bold_message(int y, int x, const char *prompt)**
- Display a bold message

**wait_for_any_input()**
- Wait for the user to type any key

**display_stats_message(int y, int x, Entry* entry)**
- Display the stats for the passed Entry pointer.

**clear_display()**
- Clears the display (temrinal)

---

## Part 4: Utilities

You will implement utility functions in `utils.c`.

### 4.1 Validate Tree Function

#### Function to Implement

```c
int check_integrity();
```

**Description:** Validates tree structure for correctness.

**Global Parameters from main.c:**
- `g_root` – pointer to root node

**Returns:**
- `1` if tree is valid, `0` otherwise

**Validation Rules:**
- Question nodes must have both children (yes and no)
- Animal nodes must have no children
- No cycles in the tree
- All nodes reachable from root

**Algorithm:**
Use BFS traversal with a queue to visit all nodes and validate structure.

**Requirements:**
- Check for NULL children on question nodes
- Check for non-NULL children on animal nodes
- Detect cycles (optional: use visited set)

---

### 4.2 Get animal Stats Function

#### Function to Implement

```c
int check_animal();
```

**Description:** Check and display active animal stats.

**Returns:**
- Sum of `timesGuessed` and `timesCorrect` for animal

**Global Parameters from main.c:**
- `g_index` - hash table for animal nodes

**Behavior**
- Prompt user to enter the animal they want to check the stats
- Display the stats if in `g_index`, otherwise tell the user the animal doesn't exist
- Return the correct value

---

## Compiling Your Code on ECE Linux Servers

If you want to test your code on the ECE Linux Servers, run the following commands:

```bash
# Compile the main program
gcc *.c -o main -lncurses -g

# Run the program
./main
```

**Compiler Flags:**
- `-lncurses` – Link the ncurses library (for GUI)
- `-g` – Include debugging information

---

## Testing Your Code

### Provided Test Cases

The `tests.c` file contains comprehensive unit tests.

### Running Tests

Modify main.c to run the `test_main()` function by uncommiting the included lines.

### Memory Leak Testing

Run Valgrind on the ECE Linux Servers to test for memory leaks. Be sure to follow the compilation steps above.

```bash
valgrind --leak-check=full ./main
```
Verify you get no `definitley` or `indireclty` lost bytes.

### Writing Your Own Tests

Create additional test cases to verify edge cases. An example test case is below. It should be called when the global EditStacks are empty.

```c
void testEmptyTreeOperations(void) {
    int result = undo_last_edit();
    assert(result == 0);     // Should return 0 (nothing to undo)
}
```


---

## GUI Features

The ncurses-based interface provides:

- **Main Menu:** [P]lay, [U]ndo, [R]edo, [S]ave, [L]oad, [I]ntegrity, [C]heck Animal [Q]uit
- **Color Coding:** Green for success, red for errors, yellow for prompts
- **Status Display:** Shows tree size and undo/redo stack counts
- **Interactive Prompts:** Easy yes/no question answering

---

## Constraints and Assumptions

- Text strings are valid UTF-8
- User input is well-formed (y/n answers)

---

## Complexity Analysis

### Time Complexity

| Operation   | Complexity | Notes                    |
|-------------|------------|--------------------------|
| Gameplay    | O(h)       | h is tree height         |
| Learning    | O(1)       | Node insertion           |
| Undo/Redo   | O(1)       | Stack operations         |
| Hash Lookup | O(1)       | Average case             |

### Space Complexity

- **Tree:** O(n) for n nodes
- **Stack:** O(h) for gameplay frames
- **Queue:** O(n) during BFS operations
- **Hash Table:** O(m + k) for m attributes and k animal mappings
- **Edit Stacks:** O(e) for e edits

---

## Common Pitfalls

**Memory Leaks in Learning**
- Remember to free old nodes when replacing them
- Don't forget to free the text strings inside nodes

**Parent Tracking**
- Root node has NULL parent (special case)
- Must capture parent in stack frames during traversal

**Undo/Redo Memory Management**
- Careful ownership: who owns which nodes?
- Don't double-free nodes

**BFS ID Assignment**
- IDs must be sequential starting from 0
- Parent must have lower ID than children

**Hash Collisions**
- Properly maintain linked lists in buckets
- Don't lose pointers when inserting

---

## Submission Instructions

1. Navigate to GradeScope via the link on Canvas
2. Upload your implementation files:
    - `ds.c`
    - `game.c`
    - `utils.c`
3. Wait for the autograder to run
4. Review your results and resubmit if necessary

---

## Getting Help

If you encounter issues:

- Attend office hours
- Post questions on Ed Discussion (without sharing code)
- Review lecture notes on trees, stacks, queues, and hash tables
- Use print statements to debug traversal logic
- Draw tree diagrams to understand structure changes
- Test with small trees first (2-3 nodes), then gradually increase complexity

---

## Summary

This lab teaches you advanced C programming concepts:

- **Binary trees** – building and traversing decision trees
- **Iterative algorithms** – using explicit stacks instead of recursion
- **Data structures** – stacks, queues, and hash tables from scratch
- **Memory management** – complex ownership with undo/redo
- **User interfaces** – building interactive terminal applications

Remember: The autograder includes hidden test cases. Develop good testing habits by thinking through edge cases and writing your own tests.

Good luck!