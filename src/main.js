function frame() {
  wasm.instance.exports.render();
  ctx.putImageData(img, 0, 0);
  requestAnimationFrame(frame);
}

async function main() {
  const canvas = document.getElementById("canvas");
  const ctx = canvas.getContext("2d");
  const width = canvas.width;
  height = canvas.height;

  const memory = new WebAssembly.Memory({
    initial: 10,
    maximum: 100,
  });

  const bytes = await (await fetch("index.wasm")).arrayBuffer();
  const module = await WebAssembly.compile(bytes);
  console.log(WebAssembly.Module.imports(module));

  let result = await WebAssembly.instantiateStreaming(fetch("index.wasm"), {
    env: {}
  });

  console.log(result);

  //
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
