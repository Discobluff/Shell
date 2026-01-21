import subprocess
import time
import os
import signal

def test_background_job():
    print("[TEST] Integration: Background Job...")
    
    # Start the shell
    process = subprocess.Popen(
        ['./main'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        bufsize=0 # Unbuffered
    )
    
    try:
        # Give it a moment to start
        time.sleep(0.5)
        
        # Send a background job
        cmd = "sleep 1 &\n"
        process.stdin.write(cmd)
        process.stdin.flush()
        
        time.sleep(0.2)
        
        # Check jobs list
        process.stdin.write("jobs\n")
        process.stdin.flush()
        
        time.sleep(0.2)
        
        # Kill shell cleanly
        process.terminate()
        stdout, stderr = process.communicate()
        
        # Verify Output
        if "[1]" in stdout and "Running" in stdout:
             print("PASSED: Job started and listed as running.")
        else:
             print("FAILED: Job not found or not running.")
             print("Output was:\n", stdout)
             exit(1)

    except Exception as e:
        print(f"FAILED with exception: {e}")
        process.kill()
        exit(1)

def test_foreground_stop():
    print("[TEST] Integration: Foreground Stop (Ctrl+Z simulation)...")
    # This is harder to test via subprocess as we need to send signals to the child of the shell.
    # We will skip complex signal injection for this simple script, 
    # but the unit tests covered the status update logic.
    print("SKIPPED (requires complex PTY handling)")

if __name__ == "__main__":
    if not os.path.exists("./main"):
        print("Error: ./main not found. Run make first.")
        exit(1)
        
    test_background_job()
    # test_foreground_stop()
    print("Integration tests finished.")
