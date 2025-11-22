import os

def collect_sources(output_filename="collected_sources.txt"):
    # 현재 파이썬 스크립트가 위치한 디렉토리 기준
    base_dir = os.path.dirname(os.path.abspath(__file__))

    # 출력 파일 오픈
    with open(output_filename, "w", encoding="utf-8") as out:
        out.write("=== Source Collection Start ===\n\n")

        # 재귀 탐색
        for root, dirs, files in os.walk(base_dir):
            for filename in files:
                # c와 h 파일만 선택
                if filename.lower().endswith((".c", ".h")):
                    full_path = os.path.join(root, filename)

                    try:
                        with open(full_path, "r", encoding="utf-8", errors="replace") as f:
                            content = f.read()
                    except Exception as e:
                        content = f"[ERROR reading file: {e}]"

                    # 파일 구분 헤더 작성
                    out.write("------------------------------------------------------------\n")
                    out.write(f"[FILE PATH] {full_path}\n")
                    out.write(f"[FILE NAME] {filename}\n")
                    out.write("------------------------------------------------------------\n\n")

                    # 파일 내용 기록
                    out.write(content)
                    out.write("\n\n")

        out.write("=== Source Collection End ===\n")

if __name__ == "__main__":
    collect_sources()
    print("모든 C/H 파일을 collected_sources.txt 로 추출 완료!")
