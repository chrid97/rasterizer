async function main() {
  const canvas = document.getElementById("canvas");
  const ctx = canvas.getContext("2d");
  const width = canvas.width;
  const height = canvas.height;

  const buffer_size_in_bytes = width * height * 4;
  const bytes_per_page = 64 * 1024;
  const initial_pages = Math.ceil(buffer_size_in_bytes / bytes_per_page);

  // (TODO) Investigate how much memory I actually
  // need so I can adjust page size later
  const memory = new WebAssembly.Memory({
    initial: 256,
    maximum: 256,
  });

  const frameBufferPtr = 0;
  const frameBuffer = new Uint8ClampedArray(memory.buffer, frameBufferPtr, buffer_size_in_bytes);

  let wasm = await WebAssembly.instantiateStreaming(fetch("index.wasm"), {
    env: { memory: memory }
  });

  console.log("Exports:", Object.keys(wasm.instance.exports));
  wasm.instance.exports.render(frameBufferPtr, buffer_size_in_bytes, width, height);
  console.log(frameBuffer[1]);
}

main();
