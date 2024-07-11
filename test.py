import subprocess


def run_interpreter(input_data):
    executable_path = '/Users/macbook/Documents/Interpreter/cmake-build-debug/main'
    process = subprocess.Popen(
        [executable_path],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    stdout, stderr = process.communicate(input_data)
    return stdout.strip(), stderr.strip()


def test_interpreter():
    tests = [
        {
            "input": "-6 + 3 * (-2)\n0\n",
            "expected_output": "Result: -12"
        },
        {
            "input": "max(5, 6)\n0\n",
            "expected_output": "Result: 6"
        },
        {
            "input": "min(5, 3)\n0\n",
            "expected_output": "Result: 3"
        },
        {
            "input": "max(min(3 * 2, 2), 2)\n0\n",
            "expected_output": "Result: 2"
        },
        {
            "input": "pow(2, max(9/3, 2))\n0\n",
            "expected_output": "Result: 8"
        },
        {
            "input": "15 + (345 - 300 / 10) + min(10, max(3 * 2, 2))\n0\n",
            "expected_output": "Result: 336"
        },
        {
            "input": "max(2*5, 4/4) * min(2, 4-1) / (3 + 2)\n0\n",
            "expected_output": "Result: 4"
        },
        {
            "input": "abs(-8)\n0\n",
            "expected_output": "Result: 8"
        },
        {
            "input": "var x = 3 * 5\nx + 2\n0\n",
            "expected_output": "Result: 17"
        },
        {
            "input": "var y = max(3, 2)\ny * 2 + 5\n0\n",
            "expected_output": "Result: 11"
        },
        {
            "input": "var z = min(4, 10) + abs(-3)\nz / 2\n0\n",
            "expected_output": "Result: 3.5"
        },
        {
            "input": "var a = 5\nvar b = 10\na + b * 2\n0\n",
            "expected_output": "Result: 25"
        },
        {
            "input": "var c = pow(2, 3)\nvar d = max(c, 5)\nd - 2\n0\n",
            "expected_output": "Result: 6"
        },
        {
            "input": "var f = 1000 * max(5, 2) + 5000 / min(2, 4) + abs(-100)\nf - 500\n0\n",
            "expected_output": "Result: 7100"
        },
        {
            "input": "var h = 2 * max(3, 7) - min(10, 4)\nh * 5 + 10 / 2\n0\n",
            "expected_output": "Result: 55"
        },
        {
            "input": "(3 + 5) * (4 - 2)\n0\n",
            "expected_output": "Result: 16"
        },
        {
            "input": "pow(2, max(9/3, 2)) - (15 + (345 - 300 / 10) + min(10, max(3 * 2, 2)))\n0\n",
            "expected_output": "Result: -328"
        },
        {
            "input": "(max(max(3, 2), pow(2,2)) / abs(-7)) * (-12 + (-2)) - (21/7)\n0\n",
            "expected_output": "Result: -11"
        },
    ]

    for test in tests:
        input_data = test["input"]
        expected_output = test["expected_output"]
        output, error = run_interpreter(input_data)

        if error:
            print(f"Test failed for input:{input_data}")
            print(f"Error:{error}")
        elif output.strip() != expected_output:
            print(f"Test failed for input:{input_data}")
            print(f"Expected:{expected_output}")
            print(f"Got:{output}")
        else:
            print(f"Test passed for input:{input_data.strip().replace('0', '')}")

        print("====================================")


if __name__ == "__main__":
    test_interpreter()
