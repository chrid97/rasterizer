function frame() {
  wasm.instance.exports.render();
  ctx.putImageData(img, 0, 0);
  requestAnimationFrame(frame);
}

async function main() {
  const canvas = document.getElementById("canvas");
  const ctx = canvas.getContext("2d");
  const width = canvas.width;
  const height = canvas.height;

  const buffer_size_in_bytes = width * height * 4;
  const bytes_per_page = 64 * 1024;
  const initial_pages = Math.ceil(buffer_size_in_bytes / bytes_per_page);

  const memory = new WebAssembly.Memory({
    initial: initial_pages,
    maximum: initial_pages,
  });

  const frameBuffer = new Uint8ClampedArray(memory.buffer, 0, buffer_size_in_bytes);

  let wasm = await WebAssembly.instantiateStreaming(fetch("index.wasm"), {
    env: { mem: memory }
  });

  console.log("Exports:", Object.keys(wasm.instance.exports));
  wasm.instance.exports.render(frameBuffer, buffer_size_in_bytes, width, height);


  // const memory = wasm.instance.exports.memory;
  // const frameBufferPtr = 0;
  //
  // const frameBuffer = new Uint8ClampedArray(memory.buffer, frameBufferPtr, width * height * 4);
  // wasm.instance.exports.set_framebuffer(framebufferPtr, width, height);
  //
  // const img = new ImageData(framebuffer, width, height);
  // frame();
}

main();
