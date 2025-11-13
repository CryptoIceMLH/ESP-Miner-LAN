const fs = require('fs');
const path = require('path');

let version;
try {
    version = require('child_process').execSync('git describe --tags --always --dirty').toString().trim();
} catch (e) {
    // Not a git repository, use default version
    version = 'v3.1.0';
    console.log('Not a git repository, using default version:', version);
}

const outputPath = path.join(__dirname, 'dist', 'axe-os', 'version.txt');
fs.writeFileSync(outputPath, version);

console.log(`Generated ${outputPath} with version ${version}`);
