#!/usr/bin/env python3
from pathlib import Path
import subprocess
import sys
import difflib

# Path to compiled executable
EXECUTABLE = Path(__file__).parent / "alg2-project"

# Directory with test input / expected output
TEST_DIR = Path(__file__).parent / "test-data"


def run_one_test(result_file: Path) -> bool:
    """Run a single test and compare program output with expected result."""
    base = result_file.name.replace("_Results.txt", "")

    platforms_file = TEST_DIR / f"{base}_OilPlatforms.txt"
    stations_file = TEST_DIR / f"{base}_PumpingStations.txt"

    missing = []
    if not platforms_file.exists():
        missing.append(platforms_file.name)
    if not stations_file.exists():
        missing.append(stations_file.name)

    if missing:
        print(f"[WARN] {base}: missing input files: {', '.join(missing)}")
        return False

    if not EXECUTABLE.exists():
        print(
            f"[ERROR] Executable '{EXECUTABLE}' does not exist. "
            f"Build your project or update EXECUTABLE in run_tests.py.",
            file=sys.stderr,
        )
        sys.exit(1)

    # Run the program with the two input files as arguments
    proc = subprocess.run(
        [str(EXECUTABLE), str(platforms_file), str(stations_file)],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,        # decode as text for us
    )

    if proc.returncode != 0:
        print(f"[FAIL] {base}: program returned exit code {proc.returncode}")
        if proc.stderr:
            print("stderr:")
            print(proc.stderr)
        return False

    # Normalize newlines and strip trailing whitespace
    output = proc.stdout.replace("\r\n", "\n").strip()
    expected = result_file.read_text(
        encoding="utf-8").replace("\r\n", "\n").strip()

    if output == expected:
        print(f"[OK]   {base}")
        return True

    print(f"[FAIL] {base}: output does not match expected result.")
    diff = difflib.unified_diff(
        expected.splitlines(),
        output.splitlines(),
        fromfile="expected",
        tofile="output",
        lineterm="",
    )
    for line in diff:
        print(line)
    return False


def main() -> int:
    if not TEST_DIR.is_dir():
        print(
            f"[ERROR] Test directory '{TEST_DIR}' does not exist.", file=sys.stderr)
        return 1

    result_files = sorted(TEST_DIR.glob("*_Results.txt"))
    if not result_files:
        print("[ERROR] No *_Results.txt files found in test-data/.", file=sys.stderr)
        return 1

    total = 0
    passed = 0
    for rf in result_files:
        total += 1
        if run_one_test(rf):
            passed += 1

    print(f"\nSummary: {passed}/{total} tests passed.")
    return 0 if passed == total else 1


if __name__ == "__main__":
    raise SystemExit(main())
