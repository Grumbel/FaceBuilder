{ stdenv
, lib
, cmake
, qt6
}:

stdenv.mkDerivation rec {
  pname = "facebuilder";
  version = "0.2.0";

  src = ./.;

  nativeBuildInputs = [
    cmake
    qt6.wrapQtAppsHook
  ];

  buildInputs = [
    qt6.qtbase
    qt6.qtsvg
  ];

  meta = with lib; {
    description = "A face-composition toy (C++/Qt6 rewrite)";
    homepage = "https://github.com/Grumbel/facebuilder";
    license = licenses.gpl3Plus;
    maintainers = with maintainers; [ ];
    platforms = platforms.linux;
    mainProgram = "facebuilder";
  };
}
