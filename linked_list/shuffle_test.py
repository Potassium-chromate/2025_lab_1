import random
import math
from collections import Counter
from itertools import permutations
from copy import deepcopy

def fisher_yates_shuffle(arr):
    arr = arr[:]  # Copy to avoid modifying original
    for i in range(len(arr) - 1, 0, -1):
        j = random.randint(0, i)
        arr[i], arr[j] = arr[j], arr[i]
    return arr

def biased_shuffle(arr):
    arr = arr[:]
    for i in range(len(arr)):
        j = random.randint(0, i)  # simulating get_unsigned16() % (i + 1)
        arr[i] = arr[j]
        arr[j] = i
    return arr

def entropy(counter):
    total = sum(counter.values())
    return -sum((count / total) * math.log2(count / total) for count in counter.values())

def run_experiment(shuffle_func, num_trials=1000000, N=4):
    base = list(range(N))
    counts = Counter()

    for _ in range(num_trials):
        shuffled = shuffle_func(base)
        counts[tuple(shuffled)] += 1
    
    return counts

def main():
    N = 7 # Small array for feasible permutations (4! = 24)
    num_trials = 1000000
    ideal_entropy = math.log2(math.factorial(N))

    print(f"Running {num_trials} trials with array of size {N} (ideal entropy = {ideal_entropy:.4f} bits)\n")

    print("🔀 Testing Fisher-Yates Shuffle...")
    fy_counts = run_experiment(fisher_yates_shuffle, num_trials, N)
    fy_entropy = entropy(fy_counts)
    print(f"Fisher-Yates Entropy: {fy_entropy:.4f} bits")
    print(f"Unique permutations: {len(fy_counts)} / {math.factorial(N)}\n")

    print("⚠️ Testing Biased Shuffle...")
    biased_counts = run_experiment(biased_shuffle, num_trials, N)
    biased_entropy = entropy(biased_counts)
    print(f"Biased Shuffle Entropy: {biased_entropy:.4f} bits")
    print(f"Unique permutations: {len(biased_counts)} / {math.factorial(N)}")

if __name__ == "__main__":
    main()

