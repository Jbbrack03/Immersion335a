// swift-tools-version: 5.9
import PackageDescription

let package = Package(
    name: "AuraMind",
    platforms: [
        .iOS(.v16)
    ],
    products: [
        .library(
            name: "AuraMind",
            targets: ["AuraMind"]
        ),
    ],
    dependencies: [
    ],
    targets: [
        .target(
            name: "AuraMind",
            dependencies: [],
            resources: [.process("Resources")]
        ),
        .testTarget(
            name: "AuraMindTests",
            dependencies: ["AuraMind"]
        ),
    ]
)