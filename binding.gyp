{
  'targets': [{
    'target_name': 'fsctl',
    'include_dirs': [
      '<!(node -e "require(\'napi-macros\')")'
    ],
    'sources': ['./binding.c'],
    'configurations': {
      'Debug': {
        'defines': ['DEBUG'],
      },
      'Release': {
        'defines': ['NDEBUG'],
      },
    },
    'conditions': [
      ['OS=="mac"', {
        'sources': [
          './src/mac.c'
        ],
      }],
      ['OS=="win"', {
        'sources': [
          './src/win.c'
        ],
      }, {
        'sources': [
          './src/posix.c',
        ],
      }],
    ],
  }]
}

