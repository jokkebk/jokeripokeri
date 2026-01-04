# Jokeri Poker Analyzer

A comprehensive toolkit for analyzing and optimizing play strategy in Jokeri Poker (Finnish video poker variant with jokers). The project calculates optimal card-keeping decisions by evaluating expected returns for all possible card selections.

## What This Project Does

Jokeri Poker is a video poker variant where players:
1. Receive 5 cards (including possible jokers)
2. Choose which cards to keep (1-5 cards)
3. Draw replacement cards for discarded ones
4. Get paid based on final hand strength

This toolkit determines which cards to keep for maximum expected return by:
- Evaluating all possible hand outcomes
- Computing win probabilities for each card selection strategy
- Providing optimal play recommendations

## Project Structure

### Core Game Logic

**Python Implementation:**
- `jokeri.py` - Poker hand evaluation engine
  - `win(hand)` - Calculates payout for a 5-card hand
  - `rawwin()` - Evaluates hands without suit information
  - Handles joker substitution logic

- `util.py` - Common utilities
  - Hand/string conversion functions
  - Combinatorics (binomial coefficients)
  - Card representation (52 regular cards + 1 joker)

- `match.py` - Pattern matching for poker hands
  - Matches hands to strategic patterns (pairs, flushes, straight draws, etc.)
  - Supports complex patterns like "open-ended straight flush draw with joker"
  - Used by strategy analysis tools

**C++ Implementation (High Performance):**
- `util.cc/h` - Core C++ utilities
  - Fast hand sorting (`sort5`, `sort4`)
  - Hand normalization and numbering
  - Win calculation
  - Combination generation

- `genhand.cc/h` - Hand generation functions
  - Generates all hands of specific types (pairs, flushes, straights, etc.)
  - Used for exhaustive strategy computation

- `jokeri.cc` - Interactive hand analyzer
  - Input a hand, get all 31 possible selections ranked by expected return

### Optimization Engines

These compute optimal play for all possible hands:

- `fastone.cc/h` - Specialized fast optimizer
  - Optimized counting for specific winning hand types (fours, sets, straight flushes)
  - Used by `jokgen.cc`

- `fastany.cc/h` - General-purpose fast optimizer
  - Works for any hand type
  - **Includes Emscripten support** for web compilation
  - Main optimization algorithm

- `jokgen.cc` - Strategy table generator
  - **Multi-threaded** optimization of all "normal" hands
  - Outputs optimal selection and expected return for each hand
  - Creates comprehensive strategy files (opti.txt format)

### Analysis & Strategy Tools

- `analyze.py` - Strategy validator
  - Tests hands against predefined strategy patterns
  - Identifies coverage gaps in strategy rules
  - Multi-processed for performance

- `stats.py` - Statistics analyzer
  - Reads optimization output files
  - Generates histograms of expected returns
  - Shows sample hands for different return levels

- `opti.py` - Data exploration tool
  - Analyzes patterns in optimal selections
  - Investigates edge cases (single-card keeps with high return, etc.)

### Machine Learning Experiment

An experimental neural network approach to learning optimal play:

- `kerastest.py` - Neural network trainer
  - Trains a deep network to predict optimal card selection
  - Input: hand encoded as one-hot vectors
  - Output: 5-bit mask of cards to keep
  - Uses binary cross-entropy loss

- `convopti.py` - Data converter
  - Converts optimization results to one-hot encoding
  - Prepares training data for neural network

- `jokeras.py` - Interactive ML predictor
  - Uses trained Keras model to suggest card selections
  - Alternative to brute-force optimization

### Test & Verification Files

- `test.cc` - Win calculation validator
  - Generates all hand types and verifies correct payouts
  - Tests the `win()` function

- `test_win.cc` - Additional win tests

- `testopti.cc` - Optimization result validator
  - Loads pre-computed optimal strategies
  - Re-calculates random hands to verify correctness
  - Computes average expected return across all hands

- `testfast.cc` - Fast optimizer tester
  - Tests `fastany` optimization engine
  - Benchmarks performance

### Build System

- `Makefile` - Compiles C++ executables
  - Targets: `test.exe`, `jokeri.exe`, `jokgen.exe`, `test_win.exe`, `testfast.exe`
  - Uses g++ with -O3 optimization
  - References MinGW threading library

## File Relationships & Dependencies

### Dependency Graph

```
Python Stack:
  jokeri.py ← match.py ← analyze.py
       ↑
    util.py → stats.py, opti.py
       ↑
  convopti.py → kerastest.py → jokeras.py

C++ Stack:
  util.cc/h → [all C++ files]
  genhand.cc/h → test.cc, jokgen.cc, testfast.cc
  fastone.cc/h → jokgen.cc
  fastany.cc/h → testfast.cc
```

### File Groupings by Purpose

**Keep if you want:**
- **Interactive analysis**: `jokeri.py`, `jokeri.cc`, `util.py`, `util.cc/h`
- **Strategy generation**: `jokgen.cc`, `fastone.cc/h`, `genhand.cc/h`, `util.cc/h`
- **Fast web optimizer**: `fastany.cc/h`, `util.cc/h` (compile with Emscripten)
- **Strategy analysis**: `analyze.py`, `match.py`, `util.py`
- **ML approach**: `kerastest.py`, `jokeras.py`, `convopti.py`, `util.py`

**Can likely remove:**
- **Statistics/exploration** (if not actively analyzing): `stats.py`, `opti.py`
- **Tests** (if core logic is verified): `test.cc`, `test_win.cc`, `testopti.cc`, `testfast.cc`

## Usage Examples

### Analyze a Hand (Python)
```bash
python3 jokeri.py
# Input hands like: 2C 3C 4C 5C 6C
```

### Analyze a Hand (C++ - faster)
```bash
make jokeri.exe
./jokeri.exe
# Input hands, get all selections ranked
```

### Generate Complete Strategy Table
```bash
make jokgen.exe
./jokgen.exe opti.txt
# Outputs: hand_number selection_mask expected_return
```

### Analyze Strategy Coverage
```bash
python3 analyze.py opti.txt
# Tests strategy patterns against computed optimal play
```

### Train Neural Network
```bash
# Convert optimization data to numpy format
python3 convopti.py opti.txt > opti3.txt
# Convert to numpy array (add conversion step)
python3 kerastest.py
```

## Technical Details

### Card Encoding
- Cards 0-51: Regular cards (value*4 + suit)
  - Values: 0=2, 1=3, ..., 12=Ace
  - Suits: 0-3 (Clubs, Diamonds, Hearts, Spades)
- Card 52: Joker

### Payout Structure
- 50: Five of a kind (with joker)
- 30: Straight flush
- 15: Four of a kind
- 8: Full house
- 4: Flush
- 3: Straight
- 2: Three of a kind or two pair
- 0: Nothing

### Hand Selection Encoding
- 5-bit mask representing which cards to keep
- Example: 0b11001 (25) = keep cards at positions 0, 3, 4

## Performance Notes

- **Python**: Good for interactive analysis and prototyping
- **C++ single-threaded**: ~10-100x faster than Python
- **C++ multi-threaded** (`jokgen.cc`): Utilizes all CPU cores
- **Complete strategy generation**: Analyzes ~2.6 million hands
- **Emscripten build**: Enables web-based optimization

## Recommendations

### If you want to KEEP the repo:
Focus on these core components:
1. **C++ optimizer** (`fastany.cc/h`, `util.cc/h`) - The most valuable code
2. **Interactive analyzer** (`jokeri.py` or `jokeri.cc`) - Useful tool
3. **Strategy generator** (`jokgen.cc`) - Creates strategy tables

Remove test files and exploratory analysis scripts.

### If you want to REMOVE the repo:
This is a complete, working Jokeri Poker analyzer. Consider:
- Is there a web version you wanted to deploy? (Emscripten support exists)
- Do you play video poker and want optimal strategy?
- Is this a portfolio piece demonstrating optimization algorithms?

If none of these apply, it's safe to archive or delete.

### Middle Ground - Extract & Archive:
Keep only:
- `fastany.cc/h` + `util.cc/h` (core optimizer)
- `README.md` (this file)
- Pre-computed `opti.txt` (if you generated one)

This preserves the valuable work while minimizing space.
