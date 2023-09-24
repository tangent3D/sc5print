// MSX SC5 file input to PCL5 WiFi printer output for ESP32-WROOM-DA
// tangent.space 2023

#ifdef TEST // Prevent code from compiling in Arduino IDE

#include "sc5print_func.c"

// Generate an executable and write output buffer as file for testing

int main(int argc, char* argv[])
{
  if (argc == 1)
  {
      printf("Usage: SC5PRINT FILE.SC5\n");
      return 1;
  }

  FILE *read_ptr = fopen(argv[1], "rb"); // Get SC5 file from argument

  // Error checking
  if (read_ptr == NULL) // Verify file exists
  {
    printf("File not found.\n");
    return 1;
  }
  fseek(read_ptr, 0, SEEK_END); // Verify file size
  if (ftell(read_ptr) == 0) // Verify file is not empty
  {
      printf("File contains no data.\n");
      fclose(read_ptr);
      return 1;
  }
  if (ftell(read_ptr) != 27143) // Verify file is exactly 27,143 bytes
  {
    fclose(read_ptr);
    printf("Invalid file size. Valid SC5 files are 27,143 bytes.\n");
    return 1;
  }

  fseek(read_ptr, 0, SEEK_SET); // Read SC5 file into input buffer
  fread(inBuffer, INPUT_BUFFER_SIZE, 1, read_ptr);

  FILE *write_ptr; // Write to file for testing
  remove("test_out.PCL"); // Delete existing output if exists
  write_ptr = fopen("test_out.PCL", "ab"); // Open and append to output file

  convert(); // Convert contents of input buffer into output buffer

  fwrite(outBuffer, 1, sizeof outBuffer, write_ptr); // Write output buffer to file

  fclose(read_ptr);
  fclose(write_ptr);
}

#endif
