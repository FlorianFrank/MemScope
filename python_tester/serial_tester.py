import serial
import time

# Set up the serial connection (adjust port and baud rate as needed)
ser = serial.Serial('/dev/tty.usbmodem1203', 115200)
time.sleep(0.2)  # Give some time for the connection to establish

# Multiline JSON string
json_data = '''{
"puf_type": "rowHammering",
"init_value": 43690,
"puf_value": 21845,
"start_addr": 0,
"stop_addr": 1000,
"tWaitAfterInit": 18,
"ce_driven": true,
"tStartDefault": 3,
"tNextWriteDefault": 4,
"tACDefault": 5,
"tASDefault": 6,
"tAHDefault": 7,
"tPWDDefault": 17,
"tDSDefault": 9,
"tDHDefault": 10,
"tNextRead": 10000,
"ceDrivenRead": true,
"tStartReadDefault": 10,
"tASReadDefault": 9,
"tAHReadDefault": 8,
"tOEDDefault": 7,
"tPRCDefault": 10,
"tCEOEEnableDefault": 0,
"tCEOEDisableDefault": 0,
"tStartAdjusted": 3,
"tNextWriteAdjusted": 4,
"tACAdjusted": 5,
"tASAdjusted": 6,
"tAHAdjusted": 7,
"tPWDAdjusted": 17,
"tDSAdjusted": 9,
"tDHAdjusted": 10,
"tStartReadAdjusted": 10,
"tASReadAdjusted": 9,
"tAHReadAdjusted": 8,
"tOEDAdjusted": 7,
"tPRCAdjusted": 10,
"tCEOEEnableAdjusted": 0,
"tCEOEDisableAdjusted": 0,
"tWaitBetweenHammering": 100,
"hammeringIterations": 4,
"hammeringDistance": 2
}\n'''

# Chunk size (adjust as needed)
#chunk_size = 64

# Send JSON string in chunks
chunks = json_data.split('\n')
for chunk in chunks:  #range(0, len(json_data), chunk_size):
   # chunk = json_data[i:i + chunk_size]
#    chunk = json_data.split('\n')
#    time.sleep(1)
    ser.write(bytearray(chunk + "\n", 'ascii'))
    print(f"Sent chunk: {chunk}")
    time.sleep(0.05)  # Short delay between chunks to avoid overwhelming the receiver

print("JSON sent!")

# Close the serial connection
# ser.close()
