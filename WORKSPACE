load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("//third_party:third_party.bzl", "third_party_deps")

git_repository(
    name = "rules_iota",
    commit = "8a302aaeba60b9eb38f6500d48249b8cbba16c73",
    remote = "https://github.com/iotaledger/rules_iota.git",
)

git_repository(
    name = "entangled",
    commit = "6ad56514a8a13fb1bf01beb10934cb9e3fd1a8a4",
    remote = "https://github.com/iotaledger/entangled.git",
)

git_repository(
    name = "served",
    commit = "757172e1d8aa7e273e800ce8ec91974c90a2a8b1",
    remote = "https://github.com/meltwater/served.git",
)

git_repository(
    name = "io_bazel_rules_docker",
    remote = "https://github.com/bazelbuild/rules_docker.git",
    tag = "v0.6.0",
)

load("@rules_iota//:defs.bzl", "iota_deps")
load("@io_bazel_rules_docker//cc:image.bzl", _cc_image_repos = "repositories")

iota_deps()

third_party_deps()

_cc_image_repos()
