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


  let wasm = await WebAssembly.instantiateStreaming(fetch("index.wasm"), {
    env: {
      memory: memory,
      js_log: (value) => console.log(value),
    }
  });

  wasm.instance.exports.render(buffer_size_in_bytes, width, height);

  const heapBase = wasm.instance.exports.get_heap_base();
  const frameBuffer = new Uint8ClampedArray(memory.buffer, heapBase, buffer_size_in_bytes);
  const image = new ImageData(frameBuffer, width, height);
  ctx.putImageData(image, 0, 0);

  console.log("completeed")
}

main();
