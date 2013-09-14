unsigned decide(unsigned char pred, unsigned a[10]) {
  if (pred > a[2]) {
    return a[3];
  }
  switch (pred) {
    case 1:
      return a[0] + a[2];
    case 2:
      return a[3] - a[4];
    case 200:
      return a[5] * a[6];
    default:
      break;
  }

  return a[9] * a[4];
}

