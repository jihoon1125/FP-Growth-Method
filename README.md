# FP-Growth-Method
FP-growth 알고리즘을 활용하여 소비자들의 물품 구매목록을 분석하여 연관성이 높은 물품간의 관계를 파악하고 B+tree에 정보를 저장하며 관리한다.
자세한 개요는 스펙문서를 참조한다.

---

## Introduction

* Fp-growth와 B+- Tree를 사용하여 상품 추천 프로그램을 구현한다.
* 이 프로그램은 장바구니 데이터에서 같이 구매한 상품들을 받아 FP-Growth를 구축한다.
* FPGrowth는 상 품들의 연관성을 Tree 구조로 저장하고 있는 FP-Tree와 상품별 빈도수 및 정보, 해당 상품과 연결된 FP-Tree의 상품 노드들을 관리하는 Header Table로 구성된다.
* FP-Growth 구축단계에서 연관된 상품들을 묶은 Frequent Pattern의 결과는 result.txt에 빈도수, 상품 순으로 저장한다.
* Frequent Pattern들이 저장된 result.txt는 BTLOAD 명령어를 통해 빈도수를 기준으로 B+-Tree에 저장이 된다.
* B+-Tree는 IndexNode와 DataNode로 구성된다. IndexNode는 DataNode를 찾기위한 Node이고 DataNode는 해당 빈도수를 가지는 Frequent Pattern들이 저장된 Node이다.


---

## Flow chart

![image](https://user-images.githubusercontent.com/67624104/118356715-947d2380-b5b1-11eb-8e90-eea61c3416cf.png)


- 가장 먼저 market.txt에서 값을 읽어온다
- FP-Tree를 생성한다.
  * 읽어온 transaction들을 기반으로 headertable을 생성한 후 내림차순으로 정렬한다.
  * transaction까지 잘 정렬한 후 FP-tree를 생성한다.
  * 모든 transaction들에 대해 tree가 만들어질 때까지 위의 과정을 반복한다.
- Frequentpattern들을 생성한다.
  * 이전에 받은 table에서 condition이었던 item들을 저장한다.
  * Conditional Tree를 기반으로 새로운 table을 생성한다.
  * 이 테이블들과 conditional transaction들을 정렬한 후 각 단계에 맞는 Conditional fp tree를 생성한다.
  * 위 과정을 끝날 때 까지 반복한다.
- 저장했던 frequent pattern들을 불러온다.
- B+ tree를 생성한다.
  * Key에 해당하는 datanode를 찾는다
  * 해당 key의 data가 존재했다면 해당 datanode에 값을 삽입한다.
  * 존재하지 않았다면 새로 값을 넣고 노드가 overflow 되는지 확인한다.
  * Overflowed 되었다면 노드를 split하고 merge하는 과정을 반복한다.
  * 위 과정을 끝날 때 까지 반복한다.
- 이후 들어오는 PRINT 요청들을 수행한다.


---


## Result

* Market.txt, command.txt, result.txt(frequentpattern 구현)

![image](https://user-images.githubusercontent.com/67624104/118360206-9222c580-b5c1-11eb-9d1c-b3369a53c595.png)



* Log.txt


![image](https://user-images.githubusercontent.com/67624104/118360229-ab2b7680-b5c1-11eb-9d5b-06acc3f06b29.png)



---


## Consideration

* Previous condition item들을 정돈시키지 않고 다른 item들과 섞어서 보냈는데 나중에 conditional FP tree를 만들 때 불필요한 반복문 호출이 많이 발생하여 프로그램 수행속도가 매우 느려짐, 따라서 인자로 최대한 정렬된 transaction과 contitional table들을 보내도록 수정하였음

* Single path가 검사되었을 때만 부분집합을 형성하였는데, 이렇게 하면 중간중간 빠진 부분집합들이 생김, 따라서 새로운 conditional FP tree와 header table을 만들 때마다 부분집합을 만들도록 구현하였다.

* 노드의 데이터 타입을 확인하기 위해 typeid.name을 사용해보려 했지만 실패, 상속시 사용할 수 있는 오버라이딩을 적극적으로 활용하자. 
