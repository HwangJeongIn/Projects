using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;
using UnityEngine.UI;

public enum EnemyState
{
    None,
    Idle,
    Attack,
    Move,
    Chase,
    Return,
    MakeAttackFormation,
    MakeDefenseFormation,
    MakeDetourFormation,
    DashAfterMakeFormation,
    Dash,
    Rush,
    MakeADetour

}

public class EnemyControl : MonoBehaviour {

    BTAI.Root aiRoot;
    private Vector3 startPosition;
    private float startAreaRange = 3.0f;
   [SerializeField] private GameObject target = null;
    public GameObject Target
    {
        get { return target; }
        set { target = value; }
    }
    [SerializeField] private List<string> targetTags = new List<string>();


    [SerializeField] private Slider enemyHpSlider;

    // enemy parameter
    private float maxHp;
    [SerializeField]private float hp;
    public float Hp
    {
        get { return hp; }
        set { hp = value; }
    }
    public float MaxHp
    {
        get { return maxHp; }
        set { maxHp = value; }
    }

    [SerializeField] private float attackPoint = 50.0f;
    [SerializeField] private float defensePoint = 50.0f;

    [SerializeField] private float power = 500.0f;
    [SerializeField]private float defaultSpeed;
    [SerializeField]private float dashSpeed;
    [SerializeField]private float rushSpeed;
    private float speed​​Duration = 3.0f;
    // 높을수록 센것
    public float DefenseRank
    {
        get { return (defensePoint); }
    }
    public float AttackRank
    {
        get { return (attackPoint); }
    }
    public float Rank
    {
        get { return (DefenseRank + AttackRank); }
    }

    [SerializeField]private bool readyToAttack = false;
    public bool ReadyToAttack
    {
        get { return readyToAttack; }
        set { readyToAttack = value; }
    }

    private bool isActive = true;
    public bool IsActive
    {
        get { return isActive; }
        set { isActive = value; }
    }
    //private Transform myself;
    private Quaternion goalRotation;

    [SerializeField]private EnemyState state = EnemyState.None;
    [SerializeField]private EnemyState prestate = EnemyState.None;
    public EnemyState State
    {
        get { return state; }
        set { state = value; }
    }
    public EnemyState Prestate
    {
        get { return prestate; }
        set { prestate = value; }
    }

    private NavMeshAgent agent;
    private Vector3 nextDestination;
    [SerializeField] private float randomRangeOffset = 60.0f;
    [SerializeField]private float randomMoveDistance = 2.5f;
    [SerializeField]private float randomRatio = 0.5f;     // 0초과 1미만 // 작을수록 더 다이나믹하게 움직인다.

  //  private float randomValue;
    public GameObject tempPos;
    public GameObject redCube;
    // private bool isChasing = false;
    // private bool isAttacking = false;

    Vector3 formationDestination;


    //private float range1;
    //private float range2;
    //private float range3;
    //private float range4;
    //public float Range1 { get { return range1; } }
    //public float Range2 { get { return range2; } }
    //public float Range3 { get { return range3; } }
    //public float Range4 { get { return range4; } }
    Queue<Vector3> moveRoute = new Queue<Vector3>();

    private PlayerControl playerControl;

    float restingSpeed = 0.08f;
    // 적이 움직이는 방식
    private Animator animator;
    [SerializeField] TrailRenderer trailRenderer;


    // 1. 연속적으로 이동해야 하는 경우 // 일정노드를 돌아야 할 경우 // idle상태 / 
    // 움직일때 연속적으로 움직여야 하는 경우에는 moveRoute(queue)에 movePoint를 차례대로 넣어주고, moveRoute(queue)기준으로 움직인다.

    // 2. 한 지점으로 이동해야하는 경우
    // 한 목적지 기준으로 움직여야 할 경우에는 moveRoute(queue)의 영향을 받지 않고 목적지 기준으로 움직여 준다.



    void Start()
    {
        if (trailRenderer)
            trailRenderer.gameObject.SetActive(false);

        agent = GetComponent<NavMeshAgent>();
        defaultSpeed = agent.speed;
        //rushSpeed = defaultSpeed * 1.5f;
        //dashSpeed = defaultSpeed * 1.3f;
        dashSpeed = defaultSpeed;
        rushSpeed = defaultSpeed;

        animator = transform.GetComponent<Animator>();

        if (target)
        {
            if(target.tag == "Player")
            {
                playerControl = target.GetComponent<PlayerControl>();
            }
        }

        maxHp = enemyHpSlider.maxValue;
        hp = enemyHpSlider.value;




        // randomValue = randomMoveDistance / randomRatio;
        startPosition = transform.position;
        //myself = transform.parent;

        nextDestination = transform.position;

        //range1 = RangeManager.instance.EnemyRange1;
        //range2 = RangeManager.instance.EnemyRange2;
        //range3 = RangeManager.instance.EnemyRange3;
        //range4 = RangeManager.instance.EnemyRange4;

        aiRoot = BTAI.BT.Root();


        aiRoot.OpenBranch
        (
            BTAI.BT.Selector().OpenBranch
            (
                // 비활성화 되어 있으면 아무것도 해주지 않는다.
                BTAI.BT.Condition(() => !isActive),

                BTAI.BT.Sequence().OpenBranch
                (
                    // 타겟이 존재할때 항상 적의 활동 범위 내인지 확인 해준다.
                    BTAI.BT.If(() => target != null).OpenBranch
                    (
                        BTAI.BT.If(DoesntExistInActivityRange).OpenBranch
                        (
                            BTAI.BT.Call(removeTarget)
                        )
                    ),

                    // 타겟이 지워졌는지 아닌지 확인
                    BTAI.BT.Condition(() => target != null),



                    BTAI.BT.Selector().OpenBranch
                    (
                        // Range1
                        // 직접 다이렉트 공격이 가능한 거리 / 같은 편이 여러명이든 아니든 그냥 공격한다.
                        BTAI.BT.If(() => (calcDistance(target.transform.position) <= RangeManager.instance.EnemyRange1)).OpenBranch
                        (
                             // 여러명이든 한명이든 상관없이 똑같이 공격
                             // BTAI.BT.Call(() => { ReadyToAttack = false; }),
                            // 만약에 한단계 전 상태가 포메이션만든후 대시하는 상태라면 > 모든적이 준비가되어있다면 모두 리셋시켜주는 함수를 호출한다.
                            BTAI.BT.If(()=> prestate == EnemyState.DashAfterMakeFormation).OpenBranch(BTAI.BT.Call(finishSetFormationIfAllEnemiesAreFighting)),
                            //BTAI.BT.If(() => { agent.Stop(); return true; }),
                            BTAI.BT.Call(attackTarget)
                        ),

                        // Range2
                        // 대시공격을 하면 닿을거리 / 한명일때는 그냥 대시공격을 하고 여러명일때는 대형을 짜서 공격을 한다.
                        BTAI.BT.If(() => (calcDistance(target.transform.position) <= RangeManager.instance.EnemyRange2) && prestate != EnemyState.MakeDetourFormation).OpenBranch
                        (
                            BTAI.BT.Selector().OpenBranch
                            (
                                // 플레이어가 본인을 인식 못했을 경우

                                // 플레이어가 본인을 인식 했을 경우


                                // 에너미 매너저에서 확인했을 때 여러명이 있는 경우
                                BTAI.BT.If(() => EnemyManager.instance.getNumberOfEnemiesWhoHaveSameTarget(target) > 1 ).OpenBranch
                                (

                                    BTAI.BT.Selector().OpenBranch
                                    (
                                        // 만약에 체력이 부족한 적이 있다면 체크하고 그에 알맞은 포메이션설정을 해준다.
                                        // 이 체력이 부족한 적은 무조건 공격상황이 아니여야한다 공격상황이면 계속 공격하고 있기 때문에 포메이션을 만들지 못한다.
                                        BTAI.BT.If(() => EnemyManager.instance.checkExistWeakenedEnemy(target) ).OpenBranch
                                        (

                                            // 방어의 경우 포메이션을 유지해야하기 때문에 주기적으로 포메이션을 갱신해준다.
                                            // 어택포메이션의 경우 한번만들어지면 바로 공격을 한다. / 방어 포메이션은 완성되고 유지하는 형식이다

                                            // 어택 포메이션처럼 방어포메이션 만드는 것도 포메이션이 안만들어졌을때만 호출하면, 한번만들어지면, 움직이지 않는다
                                            // 그렇기때문에 조건에 의해 주기적으로 호출해주는 편이 좋다.
                                            // 1. 플레이어가 움직일때 , 2. 힐링 받는 적이 모두 힐링받았을때, 3. 적이 사망했을때
                                            // 적이 사망했을때는 어차피 적무리를 향해 움직이면 알아서 적들이 다시 정렬하기 때문에 해주지 않았다.


                                            // 근데 이는 플레이어의 움직임에 따라 바뀌기 때문에 플레이어가 움직이고 있는지 확인한다음 해준다
                                            // 플레이어일때만 움직임 확인 아닐때는 어떻게할지 모르니까 주기적으로 호출
                                            BTAI.BT.If(() =>
                                            {
                                                if (target.tag == "Player")
                                                {
                                                    if (playerControl.checkMoving())
                                                        return true;
                                                    return false;
                                                }
                                                return true;
                                            }).OpenBranch
                                                (
                                                    BTAI.BT.Selector().OpenBranch
                                                    (
                                                        BTAI.BT.If(() => target.tag == "Player").OpenBranch(BTAI.BT.Call(setDefenseFormationDirectly)),
                                                        BTAI.BT.Call(setDefenseFormation)
                                                    )
                                                ),

                                            //BTAI.BT.Call(setDefenseFormation),
                                            BTAI.BT.If(() => prestate != EnemyState.MakeDefenseFormation).OpenBranch(BTAI.BT.Call(setDefenseFormationDirectly)),
                                            BTAI.BT.If(() => Vector3.Distance(transform.position, formationDestination) > 1.0f
                                           /* && calcDistance(target.transform.position) <= RangeManager.instance.EnemyRange2*/).OpenBranch
                                            (
                                                /*BTAI.BT.Call(setDefenseFormation),*/ BTAI.BT.Call(moveToFormationDestination)
                                                
                                            )

                                        ),



                                        // 체력이 충분하다면 포메이션 공격
                                        // Selector에서 처리해야 하는 작업단위를 if(true)로 묶어준다. // 이렇게안하면 한작업만 하고 빠져나온다
                                        BTAI.BT.If(()=>true).OpenBranch
                                        (

                                            // 근데 맨처음 포메이션 지정은 해줘야하므로 다른 상태에서 넘어왔을때 한번호출한다.
                                            BTAI.BT.If(() =>prestate != EnemyState.MakeAttackFormation).OpenBranch(BTAI.BT.Call(setAttackFormationDirectly)),

                                            // 바로 공격을 해야하기 때문에 포메이션이 완성되면 바로 공격하면 된다.
                                            // 현재 포지션이 포메이션 포지션이랑 비슷해질때까지 포메이션만드는 함수를 호출해준다.
                                            //BTAI.BT.While(() => Vector3.Distance(transform.position, formationDestination) > 1.0f 
                                            //&& calcDistance(target.transform.position) <= RangeManager.instance.EnemyRange2)
                                            //.OpenBranch(BTAI.BT.Call(setAttackFormation), BTAI.BT.Call(makeFormation)),

                                            // 근데 이는 플레이어의 움직임에 따라 바뀌기 때문에 플레이어가 움직이고 있는지 확인한다음 해준다
                                            // 플레이어일때만 움직임 확인 아닐때는 어떻게할지 모르니까 주기적으로 호출
                                            BTAI.BT.If(() =>
                                            {
                                                if (target.tag == "Player")
                                                {
                                                    if (playerControl.checkMoving())
                                                        return true;
                                                    return false;
                                                }
                                                return true;
                                            }).OpenBranch
                                                (
                                                    BTAI.BT.Selector().OpenBranch
                                                    (
                                                        BTAI.BT.If(() => target.tag == "Player").OpenBranch(BTAI.BT.Call(setAttackFormationDirectly)),
                                                        BTAI.BT.Call(setAttackFormation)
                                                    )
                                                ),


                                            BTAI.BT.Selector().OpenBranch
                                            (
                                                //BTAI.BT.If(() => Vector3.Distance(transform.position, formationDestination) > 1.0f).OpenBranch
                                                //(
                                                //    BTAI.BT.Call(setAttackFormation), BTAI.BT.Call(moveToFormationDestination),
                                                //    BTAI.BT.If(() => Vector3.Distance(transform.position, formationDestination) <= 2.0f).OpenBranch(BTAI.BT.Call(() => { ReadyToAttack = true; }))
                                                //),

                                                BTAI.BT.If(() => Vector3.Distance(transform.position, formationDestination) > 1.0f).OpenBranch
                                                (
                                                    BTAI.BT.Call(moveToFormationDestination)
                                                    
                                                ),

                                                // 목적지 도착한 상황
                                                // 도착 플래그가 안켜져있는 경우만 켜준다.
                                                BTAI.BT.If(() => !ReadyToAttack).OpenBranch(BTAI.BT.Call(() => { ReadyToAttack = true; })),


                                                // 포메이션위치로 위치하였다.
                                                //BTAI.BT.Call(()=> { ReadyToAttack = true; }),

                                                // 동시 공격을 위해 나머지 적들도 포메이션에 위치했는지 알아내고 동시공격을 시작하자.
                                                // 이때 미리 공격하고 있는 적에 대해서는 검사하지 않는다
                                                // 추격중인 적은 기다려준다
                                                BTAI.BT.If(() => EnemyManager.instance.checkAllEnemiesAreReadyForFight(target)
                                                && calcDistance(target.transform.position) <= RangeManager.instance.EnemyRange2)
                                                .OpenBranch
                                                (
                                                   // Range1범위 안으로 들어갈 때까지 (공격이 가능할때 까지) 접근한다.
                                                   // 접근하면 알아서 Range1으로 들어가서 공격하게 됨.

                                                   BTAI.BT.While(() => Vector3.Distance(target.transform.position, transform.position) > RangeManager.instance.EnemyRange1
                                                   && calcDistance(target.transform.position) <= RangeManager.instance.EnemyRange2).OpenBranch
                                                   (
                                                      BTAI.BT.Call(dashToTargetAfterMakeFormation)
                                                   )

                                                )
                                            )


                                        )
                                     )
                                ),
                                // 만약 에너미 매니저에서 확인했을 때 한명인 경우
                                // 항상 실행 시켜줘야 할 것들을 하나로 묶어줄때 BTAI.BT.If(()=>true).OpenBranch()를 사용
                                // 셀렉터에서는 하나라도 true가나오면 무조건 반환하기 때문에 같이 실행할 함수들을 If노드로 묶어주었다.
                                BTAI.BT.If(()=>true).OpenBranch
                                (

                                    BTAI.BT.Call(dashToTarget)
                                    
                                    //BTAI.BT.Call(chaseTarget)
                                )
                            )
                        ),

                        // Range3
                        // 플레이어가 적을 인식했는지 확인해봐야 하는 거리 / 적이 우회공격을 할 수 있는 거리 / 한명이라면 , 여러명이라면
                        BTAI.BT.If(() => (calcDistance(target.transform.position) < RangeManager.instance.EnemyRange3)).OpenBranch
                        (
                            BTAI.BT.Selector().OpenBranch
                            (
                                // 플레이어가 본인을 인식 못했을 경우
                                // 플레이어가 본인을 인식 했을 경우

                                // 에너미 매너저에서 확인했을 때 여러명이 있는 경우
                                BTAI.BT.If(() => EnemyManager.instance.getNumberOfEnemiesWhoHaveSameTarget(target) > 1).OpenBranch
                                (
                                    BTAI.BT.If(() => prestate != EnemyState.MakeDetourFormation).OpenBranch(BTAI.BT.Call(setDetourFormationDirectly)),
                                    //BTAI.BT.While(() => Vector3.Distance(target.transform.position, transform.position) > RangeManager.instance.EnemyRange1
                                    //                && calcDistance(target.transform.position) <= RangeManager.instance.EnemyRange2).OpenBranch(
                                    BTAI.BT.If(() =>
                                    {
                                        if (target.tag == "Player")
                                        {
                                            if (playerControl.checkMoving())
                                                return true;
                                            return false;
                                        }
                                        return true;
                                    }).OpenBranch
                                        (
                                            BTAI.BT.Selector().OpenBranch
                                            (
                                                BTAI.BT.If(() => target.tag == "Player").OpenBranch(BTAI.BT.Call(setDetourFormationDirectly)),
                                                BTAI.BT.Call(setDetourFormation)
                                            )
                                        )
                                ),

                                // 혼자인 경우 그냥 추격
                                BTAI.BT.Call(chaseTarget)
                            )
                        ),
                        BTAI.BT.Call(chaseTarget)
                        //// Range4
                        //BTAI.BT.If(() => (calcDistance(target.transform.position) < RangeManager.instance.EnemyRange4)).OpenBranch
                        //(
                        //    BTAI.BT.Selector().OpenBranch
                        //    (
                        //        // 플레이어가 본인을 인식 못했을 경우
                        //        // 플레이어가 본인을 인식 했을 경우
                        //        BTAI.BT.If(() => { return false; }).OpenBranch
                        //        (
                        //            // 에너미 매너저에서 확인했을 때 여러명이 있는 경우
                        //        ),
                        //        BTAI.BT.Call(chaseTarget)
                        //    )
                        //)
                        //BTAI.BT.Call(removeTarget)
                    )
                ),

                //checkReturnStartPosition
                BTAI.BT.If(()=> calcDistance(startPosition) > startAreaRange).OpenBranch
                (
                    BTAI.BT.Call(returnStartPosition)
                ),

                //checkTargetDoesntExist
                BTAI.BT.While(()=> target==null).OpenBranch
                (
                    // 대기상태일때 할것들 정하기
                    BTAI.BT.Call(idle)
                )
            
            )
        );
        
    }

    //private void OnTriggerEnter(Collider other)
    //{
    //    if(targetTags.Contains(other.tag) == true)
    //    {
    //        target = other.gameObject;
    //       // print("enter");
    //    }
    //}

    //private void OnTriggerExit(Collider other)
    //{
    //    if (other.tag == "Player")
    //    {
    //        if (target == other.gameObject)
    //            target = null;

    //        print("exit");

    //    }
    //}
    private void FixedUpdate()
    {

        //Vector3 temp = target.transform.position;
        Vector3 temp = Camera.main.transform.position;  //target.transform.position;
        temp.y = enemyHpSlider.transform.position.y;
        enemyHpSlider.gameObject.transform.LookAt(temp);

    }

    public void rest()
    {
        hp = Mathf.Lerp(hp, maxHp, Time.deltaTime * restingSpeed);
        enemyHpSlider.value = hp;
        if (hp /maxHp > .6f)
        {
            //print("unregister");
            EnemyManager.instance.unregisterRestingEnemy(this.gameObject);

            if(EnemyManager.instance.checkExistWeakenedEnemy(target) == true)
                setDefenseFormationDirectly();
        }
    }

    void Update()
    {
        aiRoot.Tick();

        // 루트큐
        if (moveRoute.Count != 0)
        {
            if (Vector3.Distance(nextDestination, transform.position) > 1.0f)
                return;

            Vector3 destination = moveRoute.Dequeue();

            setDestination(destination);

            //destination.y = transform.position.y;
            //agent.SetDestination(destination);
            //tempPos.transform.position = destination;
            //nextDestination = destination;
        }

        // 애니메이션
        if (animator)
        {
            switch (state)
            {
                case EnemyState.Attack:
                    animator.SetBool("attacking", true);
                    break;
                default:
                    animator.SetBool("attacking", false);
                    break;
            }
            animator.SetFloat("speed", agent.velocity.magnitude);
        }
        if(target)
        {
            if(agent.velocity.sqrMagnitude == 0)
            {
                Quaternion temp = Quaternion.LookRotation((target.transform.position - transform.position));
                Quaternion temp2 = Quaternion.Euler(transform.rotation.eulerAngles.x, temp.eulerAngles.y, transform.rotation.eulerAngles.z);
                if ((int)temp.eulerAngles.y != (int)transform.rotation.eulerAngles.y)
                {
                    // LookRotation // 벡터 > 쿼터니언
                    // Euler // 오일러 > 쿼터니언
                    transform.rotation = Quaternion.Slerp(transform.rotation, temp2, Time.deltaTime * 10.0f);
                }
            }

        }

        //if(target)
        //    transform.LookAt(target.transform.position);
        //extraRotation();
    }


    IEnumerator cSetSpeedForAShortMoment(float speed, float sec)
    {
        agent.speed = speed;
        if (trailRenderer && speed >=dashSpeed) trailRenderer.gameObject.SetActive(true);
        yield return new WaitForSeconds(sec);
        if (trailRenderer && speed >= dashSpeed) trailRenderer.gameObject.SetActive(false);
        agent.speed = defaultSpeed;
    }

    Coroutine cTemp = null;

    private void setSpeedForAShortMoment(float speed,float sec)
    {
        if(cTemp!=null)
            StopCoroutine(cTemp);
        cTemp = StartCoroutine(cSetSpeedForAShortMoment(speed, sec));
    }

    private void setDestination(Vector3 destination)
    {
        destination.y = transform.position.y;
        agent.SetDestination(destination);
        tempPos.transform.position = destination;
        nextDestination = destination;
    }

    // 클리어하고 새로운 루트를 짜기 위해서 새로운 movePoint를 생성하기 위해서 사용한다.
    // 거의 한번만 호출할때 사용한다. 여러번호출되면 update()에서 setDestination하기전에 다시 클리어 될 가능성이 있다.
    // 여러번호출되면 노드를 넣었다가 클리어 시키고 넣었다가 클리어 시키고 그런식으로 실행된다.
    private void clearMoveRouteAndRegisterMovePoint(Vector3 movePoint)
    {
        if (moveRoute.Count != 0)
            clearMoveRoute();

        moveRoute.Enqueue(movePoint);
    }

    // moveRoute큐의 영향을 안받고 움직이고 싶을 때 사용한다. 
    // 현재있는 moveRoute큐가 비어있지 않다면 큐를 모두 클리어 시켜주고 다음 목표지점을 바로 설정해서 이동해준다.
    private void clearMoveRouteAndSetDestination(Vector3 movePoint)
    {
        if(moveRoute.Count !=0)
            clearMoveRoute();

        setDestination(movePoint);
    }

    private void registerMovePoint(Vector3 movePoint)
    {
        moveRoute.Enqueue(movePoint);
    }

    private void clearMoveRoute()
    {
        moveRoute.Clear();
        nextDestination = transform.position;
    }

    public void getDamaged(float damage)
    {
        hp -= damage;
        // 만약에 데미지를 받은 후에 hp가 0이하라면 슬라이더 값을 최신화 시켜주지 않는다.
        // 바로 에너미 매니저에서 현재 에너미가 등록되어 있으면 삭제하고, destroy시켜준다.
        if (hp <= 0)
        {
            removeTarget();
            Destroy(this.gameObject, 0.0f);
            return;
        }
        enemyHpSlider.value = hp;

    }


    private void finishSetFormationIfAllEnemiesAreFighting()
    {
        if (target == null) return;
        EnemyManager.instance.finishSetFormationIfAllEnemiesAreFighting(target);
        print("finishSetFormationIfAllEnemiesAreFighting");
    }

    private bool existsInActivityRange()
    {
        return Vector3.Distance(transform.position, startPosition) < RangeManager.instance.EnemyRange3 * 3;
    }

    private bool DoesntExistInActivityRange()
    {
        return Vector3.Distance(transform.position, startPosition) > RangeManager.instance.EnemyRange3 * 3;
    }

    public void removeTarget()
    {

        EnemyManager.instance.unregisterEnemyWhoDoesntHaveTarget(gameObject);
        target = null;
        playerControl = null;
    }

    public void setTarget(GameObject target)
    {
        if (!target) return;
        this.target = target;
        if(target.tag == "Player")
        {
            playerControl = target.GetComponent<PlayerControl>();
        }
        EnemyManager.instance.registerEnemyWhoHasTarget(gameObject);
    }

    private float calcDistance(Vector3 goalPosition)
    {
      //  print("f : calcDistance");
        Vector3 temp = transform.position - goalPosition;
        
        return Mathf.Sqrt(Mathf.Pow(temp.x,2) + Mathf.Pow(temp.y,2) + Mathf.Pow(temp.z, 2));
    }
 
    private void idle()
    {
        //  print("f : idle");
        setState(EnemyState.Idle);
    }

    private void setAttackFormation()
    {
        //state = EnemyState.MakeFormation;
        setState(EnemyState.MakeAttackFormation);
        EnemyManager.instance.setFormation(target, FormationType.AttackFormation,false);
    }

    private void setAttackFormationDirectly()
    {
        //state = EnemyState.MakeFormation;
        setState(EnemyState.MakeAttackFormation);
        EnemyManager.instance.setFormation(target, FormationType.AttackFormation, true);
    }

    private void setDefenseFormation()
    {
        //state = EnemyState.MakeFormation;
        setState(EnemyState.MakeDefenseFormation);
        EnemyManager.instance.setFormation(target, FormationType.DefenseFormation,false);
    }

    private void setDefenseFormationDirectly()
    {
        //state = EnemyState.MakeFormation;
        setState(EnemyState.MakeDefenseFormation);
        EnemyManager.instance.setFormation(target, FormationType.DefenseFormation,true);
    }

    private void setDetourFormationDirectly()
    {
        setState(EnemyState.MakeDetourFormation);
        EnemyManager.instance.setFormation(target, FormationType.DetourFormation, true);
    }

    private void setDetourFormation()
    {
        setState(EnemyState.MakeDetourFormation);
        EnemyManager.instance.setFormation(target, FormationType.DetourFormation, false);
    }


    // 엑스트라 스피드를 추가하든, 스피드 자체를 올리든
    private void dashToTarget()
    {
        
        setSpeedForAShortMoment(dashSpeed, speed​​Duration);
        setState(EnemyState.Dash);
        Vector3 nearTargetPoint = target.transform.position + (-target.transform.position + transform.position).normalized * 2.0f;
        clearMoveRouteAndSetDestination(nearTargetPoint);
    }

    private void dashToTargetAfterMakeFormation()
    {

        setSpeedForAShortMoment(dashSpeed, speed​​Duration);
        setState(EnemyState.DashAfterMakeFormation);
        Vector3 nearTargetPoint = target.transform.position + (-target.transform.position + transform.position).normalized * 2.0f ;
        clearMoveRouteAndSetDestination(nearTargetPoint);
    }



    public void rushToTarget()
    {
        //setState(EnemyState.Rush);

        setSpeedForAShortMoment(rushSpeed, speed​​Duration);
        setState(EnemyState.Rush);
        Vector3 nearTargetPoint = target.transform.position + (-target.transform.position + transform.position).normalized * 2.0f;
        clearMoveRouteAndSetDestination(nearTargetPoint);
    }

    public void makeADetour()
    {
        // 먼저 싹 비워준다.
        clearMoveRoute();
        // 이동하던것을 멈춘다.
        setDestination(transform.position);
        float targetRange1 = RangeManager.instance.PlayerRange1;
        Vector3 centerPos = target.transform.position + target.transform.rotation * Vector3.forward * targetRange1*2;
        Vector3 firstDestination = target.transform.position - target.transform.rotation * Vector3.forward * targetRange1*2;
        Vector3 secondDestination = target.transform.position + (-target.transform.position + firstDestination).normalized * 2.0f;

        if (moveRoute.Contains(secondDestination) == true)
            return;

        moveAround(firstDestination, target.transform.position);
        // 마지막에 최종 목표지점을 넣어준다
        registerMovePoint(secondDestination);
    }


    // 포메이션 정하는것은 디펜스 포인트에따라서 정해지는 위치가 달라지기 때문에
    // 디펜스 포인트를 전체적으로 참조할 수 있는 에너미 매니저에서 해준다. 
    //// 거기에서 컴포넌트받아와서 내부적인 작업(멤버변수에 formationDestination만들어서 매니저에서 초기화)을 다해줘야함에도 그런식으로 해놨다.
    // 하지만 우회하는 것은 현재위치 기준으로 계산해주는 경로는 똑같기 때문에 굳이 그렇게 하지 않았다.
    //
    public void setFormationDestination(Vector3 destination)
    {
        formationDestination = destination;
    }

    private void chaseTarget()
    {
        setState(EnemyState.Chase);

        if (target == null) return;
        moveWithNoise(target.transform.position);
    }
    private void moveToFormationDestination()
    {
        if (target == null) return;
        moveBehind(formationDestination, (c, t, o) => { return Vector3.Distance(c, t) > o; });
    }

    private bool checkWhetherMoveForward(float currentDistance, float tempDistance) { return currentDistance > (tempDistance + (randomMoveDistance * randomRatio)); }
    private Vector3 searchMovePoint(Vector3 goTo)
    {
        // 만약에 아직 도착못했다면 가장거리가 먼 지점을 반환한다.
        if (Vector3.Distance(nextDestination, transform.position) > 1.0f)
        {
            //print("transform.position : " + transform.position);
            //print("nextDestination : " + nextDestination);
            //print("why?");
            return new Vector3(Mathf.Infinity, Mathf.Infinity, Mathf.Infinity);
        }
        // float currentRotationY = transform.rotation.eulerAngles.y;

        // 현재 위치에서 플레이어 위치의 로테이션이다.

        Vector3 toTargetDirection = (target.transform.position - transform.position).normalized;
        Quaternion toTargetRotation = Quaternion.LookRotation(toTargetDirection);
        // 현재는 -30 ~ 30으로 랜덤값을 주었지만 나중에 정규분포로 바꿔주자.
        Quaternion tempRotation = 
            Quaternion.Euler(/*toTargetRotation.eulerAngles +*/ new Vector3(0, Random.RandomRange(-randomRangeOffset, randomRangeOffset), 0));

        // 이 벡터를 tempRotation만큼 돌려준다.
        toTargetDirection = tempRotation * toTargetDirection;
        //Quaternion.

        // 더 랜덤 값을 뽑는 방식 가까이 간다는 것을 성공이라고 가정했을때 실패할 가능성이 높다.
        //Quaternion tempRotation = Quaternion.Euler(transform.rotation.eulerAngles
        //    + new Vector3(0, Random.RandomRange(0, 360), 0));
        Vector3 temp = transform.position + (toTargetDirection * randomMoveDistance);
        temp.y = transform.position.y;
        redCube.transform.position = temp;
        //print("redCube : " + redCube.transform.position);
        return temp;
    }

    private void moveWithNoise(Vector3 goTo)
    {
        //print("실행");

        Vector3 temp = searchMovePoint(goTo);


        float fromCurrentPointToDestination = Vector3.Distance(transform.position, goTo);
        float fromTempPointToDestination = Vector3.Distance(temp, goTo);

        // 아직 도착못한경우
        if (fromTempPointToDestination == Mathf.Infinity) return;


        //print("도착해서 다음 포지션이 결정됨");
        
        //print("temp : " + temp);
        if (checkWhetherMoveForward(fromCurrentPointToDestination, fromTempPointToDestination))
        {
            //print("setDestination : " + temp);
            registerMovePoint(temp);
        }

    }

    public delegate bool Constrain(Vector3 currentPos, Vector3 targetPos ,float offset);
    /*
    private bool funcA(Vector3 currentPos, Vector3 targetPos, float offset)
    {
        return Vector3.Distance(currentPos, targetPos) > offset;
    }

    //static private Constrain constrainByDefault = (c, t, o) => Vector3.Distance(c, t) > o;

    private void moveWithConstrain(Vector3 goTo, Constrain constrain)
    {
        //state = EnemyState.;
        Vector3 temp = searchMovePoint(goTo);

        float fromCurrentPointToDestination = Vector3.Distance(transform.position, goTo);
        float fromTempPointToDestination = Vector3.Distance(temp, goTo);

        if (fromTempPointToDestination == Mathf.Infinity) return;

        // 접근중인지 확인하고 제한되는 무브포인트인지 확인한다.
        if (checkWhetherMoveForward(fromCurrentPointToDestination, fromTempPointToDestination) 
            && constrain(temp, target.transform.position, RangeManager.instance.PlayerRange1) == true)
            registerMovePoint(temp);
    }
    */
    // moveWithConstraintSmoothly
    // moveBehind
    private void moveAround( Vector3 destination, Vector3 centerPos)
    {
        // 먼저 싹 비워준다.
        clearMoveRoute();
        // 이동하던것을 멈춘다.
        setDestination(transform.position);

        float radius = Vector3.Distance(centerPos, destination);
        Vector3 directionFromCenterToCurrentPos = (transform.position - centerPos).normalized;
        Vector3 directionFromCenterToDestination = (destination - target.transform.position).normalized;

        // 제한된 범위를 빠져나갈 수 있는 노드를 설정해준다.
        Vector3 temp = target.transform.position +
            directionFromCenterToCurrentPos * (radius + 1.0f);


        float fullAngle = Quaternion.FromToRotation(directionFromCenterToCurrentPos, directionFromCenterToDestination).eulerAngles.y;
        if (fullAngle > 180)
            fullAngle = fullAngle - 360;

        // print("fullAngle1 : " + Quaternion.FromToRotation(direction, goToDirection).eulerAngles.y);
        // print("fullAngle2 : " + Quaternion.FromToRotation(goToDirection, direction).eulerAngles.y);
        int count = Mathf.Abs((int)(fullAngle / 10.0f));
        //print("fullAngle : " + fullAngle);
        //print("count : " + count);

        // 0일때는 아예 angle이 무한대값이 나온다.
        if (count != 0)
        {
            float angle = fullAngle / count;
            // print("angle : " + angle);
            for (int i = 4; i < count; ++i)
            {
                temp = target.transform.position + Quaternion.Euler(new Vector3(0, angle * (i + 1), 0))
                    * directionFromCenterToCurrentPos * (radius + 1.0f);
                registerMovePoint(temp);
            }
        }


        // 마지막에 최종 목표지점을 넣어준다
        registerMovePoint(destination);
    }

    private void moveBehind(Vector3 goTo, Constrain constrain)
    {
        // 처음 다음 목표지점이 설정안된 경우만 설정해준다.
        // 만약에 목표지점이 달라진다면 새롭게 짜줘야 한다.

        // 함수로
        if (moveRoute.Count == 0)
        {
            // moveRoute의 크기가 0일때 즉 movePoint가 없을때는 무조건 movePoint를 등록해준다.
            registerMovePoint(goTo);
        }
        else
        {
            // 만약에 moveRoute의 Count가 1개라면 그값이 에전에 설정된 formationDestination값이다.
            // 현재 formationDestination이랑 비교해서 만약 다르면 지우고 현재 formationDestination으로 목표지점을 바꿔준다.

            // 만약에 moveRoute의 Count가 1개초과라면 마지막 노드가 예전에 설정된 formationDestination값이다.
            // 현재 formationDestination이랑 비교해서 만약 다르면 노드를 모두 지워주고 현재 formationDestination으로 목표지점을 바꿔준다.
            if (moveRoute.Contains(goTo) == false)
            {
                clearMoveRouteAndRegisterMovePoint(goTo);
            }
        }


        // moveBehind(formationDestination, (c, t, o) => { return Vector3.Distance(c, t) > o; });

        // 기능분리

        // 만약 제한범위 안으로 들어온다면
        // 현재 2개의 노드를 가지고 있지 않고, 목표지점하나만을 노드로 가지고 있을 때
        if (constrain(transform.position, target.transform.position /*+ transform.forward*/, RangeManager.instance.PlayerRange1) == false && moveRoute.Count <= 1)
        {
            moveAround(goTo, target.transform.position);
        }


    }
    
    private void attackTarget()
    {
        // print("f : attackTarget");
        //setSpeedForAShortMoment(0, speed​​Duration);
        setState(EnemyState.Attack);

        //target.GetComponent<Rigidbody>().AddForce(myself.transform.forward/* + (myself.transform.up * 0.2f )*/ * power);
    }

    private void returnStartPosition()
    {
        // print("f : return StartPosition");
        setState(EnemyState.Return);
        //Vector3 toTarget = (startPosition - transform.position).normalized;
        //goalRotation = Quaternion.LookRotation(toTarget, Vector3.up);

        //myself.position += transform.forward * Time.deltaTime * moveSpeed;
        //--------------------------------------------------------------------------------------
        clearMoveRouteAndSetDestination(startPosition);
    }

    // Update is called once per frame


    //[SerializeField] float extraRotationSpeed = 500.0f;

    //void extraRotation()
    //{
    //    Vector3 lookrotation = agent.steeringTarget - transform.position;
    //    transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(lookrotation), extraRotationSpeed * Time.deltaTime);

    //}
    public void setState(EnemyState state)
    {
        prestate = this.state;
        this.state = state;
        TextControl.setEnemyState(state);
    }

    private void OnDrawGizmos()
    {
        Gizmos.color = Color.red;
        UnityEditor.Handles.color = Color.red;
        UnityEditor.Handles.DrawWireDisc(transform.position, transform.up, RangeManager.instance.EnemyRange1);
        UnityEditor.Handles.DrawWireDisc(transform.position, transform.up, RangeManager.instance.EnemyRange2);
        UnityEditor.Handles.DrawWireDisc(transform.position, transform.up, RangeManager.instance.EnemyRange3);
        //UnityEditor.Handles.DrawWireDisc(transform.position, transform.up, RangeManager.instance.EnemyRange4);
        if (target !=null)
            Gizmos.DrawLine(transform.position, target.transform.position);


        Gizmos.color = Color.cyan;
        if (moveRoute.Count >= 1)
        {
            Vector3 startPoint = transform.position;
            List<Vector3> temp = new List<Vector3>(moveRoute);
            for (int i = 0; i < temp.Count; ++i)
            {
                Gizmos.DrawLine(startPoint, temp[i]);
                startPoint = temp[i];
            }
        }
    }




    


    //private bool checkReturnStartPosition()
    //{
    //    if (calcDistance(startPosition) > startAreaRange)
    //    {
    //        return true;
    //    }
    //    return false;
    //}

    //private bool checkTargetExist()
    //{
    //   // print("f : checkTargetExist");

    //    if (target == null)
    //        return false;

    //    return true;
    //}

    //private bool checkTargetDoesntExist()
    //{
    //    // print("f : checkTargetDoesntExist");

    //    if (target == null)
    //        return true;

    //    return false;
    //}


    //private bool checkCanChaseTarget()
    //{
    //    //  print("f : checkCanChaseTarget");
    //    if (target == null)
    //        return false;

    //    if (calcDistance(target.transform.position) < chaseRange)
    //    {
    //        return true;
    //    }

    //    target = null;
    //    return false;
    //}

    //private bool checkCanAttackTarget()
    //{
    //    //  print("f : checkCanAttackTarget");
    //    if (target == null)
    //        return false;

    //    if (calcDistance(target.transform.position) < attackRange)
    //    {
    //        return true;
    //    }

    //    return false;

    //}

}
