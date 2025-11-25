async function main() {
  const canvas = document.getElementById("canvas");
  // TODO error if no canvas is found
  const ctx = canvas.getContext("2d");
  // TODO error if can't create context
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

  // WASM linear memory page 0 (0–64KB) is used by compiler/runtime: globals, stack, metadata.
  // Start our framebuffer at page 1 (offset 64KB) to avoid overwriting internal state.
  const frameBufferPtr = 1024 * 64;
  const frameBuffer = new Uint8ClampedArray(memory.buffer, frameBufferPtr, buffer_size_in_bytes);

  let wasm = await WebAssembly.instantiateStreaming(fetch("index.wasm"), {
    env: {
      memory: memory,
    }
  });

  wasm.instance.exports.render(frameBufferPtr, buffer_size_in_bytes, width, height);

  const image = new ImageData(frameBuffer, width, height);
  ctx.putImageData(image, 0, 0);

  console.log("completeed")
}

main();
